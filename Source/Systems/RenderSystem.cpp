#include "RenderSystem.hpp"

#include "Components/ModelComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "Components/TransformComponent.hpp"

#include <algorithm>
#include <ranges>

#include <glm/gtc/matrix_transform.hpp>

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
    mUploads.reserve(30'000);

    eventManager->Subscribe<fra::KeyPressedEvent>([this](const fra::KeyPressedEvent& event) {
        if (event.key != fra::KeyCode::F2)
            return;

        mEnabled = !mEnabled;
    });
}

void RenderSystem::PostUpdate(float /*dt*/)
{
    if (!mEnabled)
        return;

    BeginFrame();
    SubmitScene();
    EndFrame();
}

void RenderSystem::BeginFrame()
{
    mRegistry->BeginTrace("WaitForAllTasks");
    mThreadPool->WaitForAllTasks();
    mRegistry->EndTrace();

    mRegistry->BeginTrace("Freya BeginFrame");
    mRenderer->BeginFrame();
    mRegistry->EndTrace();

    if (mPlayer.has_value())
        mRegistry->TryGetComponents<TransformComponent>(mPlayer.value(), [this](const TransformComponent& transform) {
            const auto cameraPosition =
                transform.position - transform.GetForwardDirection() * 15.0f + transform.GetUpDirection() * 4.0f;

            const auto cameraTarget =
                transform.position + transform.GetForwardDirection() * 1500.0f;
            const auto cameraUp = transform.GetUpDirection();

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
    mRegistry->BeginTrace("Clear buffers");
    mRenderables.clear();
    mUploads.clear();
    mRegistry->EndTrace();

    mRegistry->BeginTrace("Query renderables");
    mOctreeSystem->Query(Frustum(mViewProj), mRenderables);
    mRegistry->EndTrace();

    mRegistry->BeginTrace("Collect scene instances");
    mUploads.reserve(mRenderables.size());
    for (const auto& renderable : mRenderables)
    {
        mRegistry->TryGetComponents<ModelComponent, TransformComponent>(
            renderable.entity, [&](const ModelComponent& model, const TransformComponent& transform) {
                if (model.meshes == nullptr || model.meshes->empty())
                    return;

                const auto modelMatrix = transform.GetModel();
                for (const auto meshId : *model.meshes)
                {
                    mUploads.push_back(fra::SceneInstanceUpload {
                        .model       = modelMatrix,
                        .meshId      = meshId,
                        .materialId  = model.material,
                        .entityId    = renderable.entity,
                        .castShadows = true,
                    });
                }
            });
    }
    mRegistry->EndTrace();

    if (mUploads.empty())
        return;

    mRegistry->BeginTrace("Sort scene instances");
    std::ranges::sort(mUploads, [](const fra::SceneInstanceUpload& a, const fra::SceneInstanceUpload& b) {
        return a.entityId < b.entityId;
    });
    mRegistry->EndTrace();

    mRegistry->BeginTrace("UploadSceneInstances");
    mRenderer->UploadSceneInstances(mUploads);
    mRegistry->EndTrace();
}

void RenderSystem::EndFrame() const
{
    mRegistry->BeginTrace("Render");
    mRenderer->EndFrame();
    mRegistry->EndTrace();
}
