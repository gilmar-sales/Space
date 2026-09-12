#include "RenderSystem.hpp"

#include "Components/HealthComponent.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "Components/SquadComponent.hpp"
#include "Components/TransformComponent.hpp"

#include <Freya/Asset/SceneInstanceUpload.hpp>

#include <algorithm>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

namespace
{
constexpr glm::vec4 HealthBarBackground { 0.08f, 0.08f, 0.08f, 0.85f };
constexpr glm::vec4 AllyHealthForeground { 0.25f, 0.85f, 0.35f, 1.0f };
constexpr glm::vec4 EnemyHealthForeground { 0.90f, 0.25f, 0.20f, 1.0f };
constexpr float     HealthBarWidth   = 6.0f;
constexpr float     HealthBarHeight  = 0.55f;
constexpr float     HealthBarYOffset = 4.0f;

constexpr std::size_t UploadChunkSize = 2'048;

/// Match TransformComponent::GetModel (T * inverse(R) * S) with Freya SceneTransform expand.
fra::SceneTransform ToSceneTransform(const TransformComponent& transform)
{
    return fra::SceneTransform {
        .position = transform.position,
        .scale    = transform.scale,
        .rotation = glm::inverse(transform.rotation),
    };
}
} // namespace

RenderSystem::RenderSystem(const skr::Arc<fr::Registry>& registry, const skr::Arc<fra::Renderer>& renderer,
                           const skr::Arc<fra::Window>& window, const skr::Arc<fra::MeshPool>& meshPool,
                           const skr::Arc<fra::MaterialPool>& materialPool,
                           const skr::Arc<OctreeSystem>& octreeSystem, const skr::Arc<fr::ThreadPool>& taskManager,
                           const skr::Arc<fra::EventManager>& eventManager) :
    System(registry), mRenderer(renderer), mWindow(window), mMaterialPool(materialPool), mMeshPool(meshPool),
    mOctreeSystem(octreeSystem), mThreadPool(taskManager), mEnabled(true)
{
    mPlayer = mRegistry->CreateQuery()->FindUnique<PlayerComponent>();
    mRenderables.reserve(30'000);

    eventManager->Subscribe<fra::KeyPressedEvent>([this](const fra::KeyPressedEvent& event) {
        if (event.key != fra::KeyCode::F2)
            return;

        mEnabled = !mEnabled;
    });
}

void RenderSystem::PostUpdate(float dt)
{
    if (!mEnabled)
        return;

    BeginFrame();
    SubmitScene();
    SubmitHealthBars();
    EndFrame();
}

void RenderSystem::BeginFrame()
{
    mRegistry->BeginTrace("ExecuteTasks");
    mRegistry->ExecuteTasks();
    mRegistry->EndTrace();

    mRegistry->BeginTrace("Freya BeginFrame");
    mRenderer->BeginFrame();
    mRegistry->EndTrace();

    if (!mPlayer.has_value())
        mPlayer = mRegistry->CreateQuery()->FindUnique<PlayerComponent>();

    if (mPlayer.has_value())
        mRegistry->TryGetComponents<TransformComponent>(mPlayer.value(), [this](const TransformComponent& transform) {
            const auto cameraPosition =
                transform.position - transform.GetForwardDirection() * 15.0f + transform.GetUpDirection() * 4.0f;

            const auto cameraTarget = transform.position + transform.GetForwardDirection() * 1500.0f;
            const auto cameraUp     = transform.GetUpDirection();

            constexpr float fovRadians = glm::radians(45.0f);
            constexpr float nearPlane  = 1.0f;
            const float     farPlane   = mRenderer->GetDrawDistance();
            const float     aspect =
                static_cast<float>(mWindow->GetWidth()) / static_cast<float>(std::max(mWindow->GetHeight(), 1u));

            mRenderer->UpdateCamera(cameraPosition, cameraTarget, cameraUp, fovRadians, nearPlane, farPlane);

            const auto view       = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
            const auto projection = mRenderer->MakeProjection(fovRadians, aspect, nearPlane, farPlane);
            mViewProj             = projection * view;
        });
}

void RenderSystem::SubmitScene()
{
    mRegistry->BeginTrace("Octree frustum query");
    mRenderables.clear();
    mOctreeSystem->Query(Frustum(mViewProj), mRenderables);
    mRegistry->EndTrace();

    mRegistry->BeginTrace("BeginSceneInstances");
    mRenderer->BeginSceneInstances();
    mRegistry->EndTrace();

    mRegistry->BeginTrace("ReserveSceneInstances");
    mRenderer->ReserveSceneInstances(std::max(mInstanceReserve, static_cast<std::uint32_t>(mRenderables.size())));
    mRegistry->EndTrace();

    std::uint32_t uploaded = 0;

    mRegistry->BeginTrace("UploadSceneInstances");
    std::vector<fra::SceneInstanceUpload> uploads;
    uploads.reserve(UploadChunkSize);

    for (std::size_t start = 0; start < mRenderables.size(); start += UploadChunkSize)
    {
        uploads.clear();
        const auto end = std::min(start + UploadChunkSize, mRenderables.size());

        for (std::size_t i = start; i < end; ++i)
        {
            const auto& renderable = mRenderables[i];
            // Live transform (not octree snapshot): camera already uses the
            // current pose; drawing from PreUpdate particles made the chase
            // cam feel one physics step behind the ship.
            mRegistry->TryGetComponents<ModelComponent, TransformComponent>(
                renderable.entity,
                [&](const ModelComponent& model, const TransformComponent& transform) {
                    if (model.meshes == nullptr || model.meshes->empty())
                        return;

                    const auto sceneTransform = ToSceneTransform(transform);
                    for (const auto mesh : *model.meshes)
                    {
                        uploads.push_back(fra::SceneInstanceUpload {
                            .transform = sceneTransform,
                            .mesh      = mesh,
                            .material  = model.material,
                            .entityId  = static_cast<std::uint32_t>(renderable.entity),
                            .flags     = fra::MakeSceneInstanceFlags(true),
                        });
                    }
                });
        }

        if (uploads.empty())
            continue;

        uploaded += static_cast<std::uint32_t>(uploads.size());
        mRenderer->UploadSceneInstances(uploads);
    }
    mRegistry->EndTrace();

    mRegistry->BeginTrace("EndSceneInstances");
    mRenderer->EndSceneInstances();
    mRegistry->EndTrace();

    if (uploaded > mInstanceReserve)
        mInstanceReserve = uploaded;
}

void RenderSystem::SubmitHealthBars()
{
    mRegistry->BeginTrace("HealthBars");
    auto& draw = mRenderer->GetBillboardDraw();

    for (const auto& renderable : mRenderables)
    {
        if (mPlayer.has_value() && renderable.entity == mPlayer.value())
            continue;

        mRegistry->TryGetComponents<TransformComponent, HealthComponent, SquadComponent>(
            renderable.entity,
            [&](const TransformComponent& transform, const HealthComponent& health, const SquadComponent& squad) {
                if (health.maxHitPoints <= 0.0f)
                    return;

                const float fill = std::clamp(health.hitPoints / health.maxHitPoints, 0.0f, 1.0f);
                const auto  fg   = squad.squad == Squad::Ally ? AllyHealthForeground : EnemyHealthForeground;
                const auto  head = transform.position + transform.GetUpDirection() * HealthBarYOffset;

                draw.HealthBar(head, HealthBarWidth, HealthBarHeight, fill, HealthBarBackground, fg,
                               fra::BillboardAlign::Screen);
            });
    }
    mRegistry->EndTrace();
}

void RenderSystem::EndFrame()
{
    mRegistry->BeginTrace("Render");
    mRenderer->EndFrame();
    mRegistry->EndTrace();
}
