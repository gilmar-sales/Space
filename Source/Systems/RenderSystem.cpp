#include "RenderSystem.hpp"

#include "Components/ModelComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "Components/TransformComponent.hpp"

#include <algorithm>
#include <ranges>

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

void RenderSystem::BeginFrame() const
{
    mThreadPool->WaitForAllTasks();
    mRenderer->BeginFrame();

    if (mPlayer.has_value())
        mRegistry->TryGetComponents<TransformComponent>(mPlayer.value(), [this](const TransformComponent& transform) {
            const auto cameraPosition =
                transform.position - transform.GetForwardDirection() * 15.0f + transform.GetUpDirection() * 4.0f;

            const auto cameraForward =
                glm::normalize(transform.position + transform.GetForwardDirection() * 1500.0f - cameraPosition);

            mRenderer->UpdateCamera(cameraPosition, cameraPosition + cameraForward, transform.GetUpDirection());
        });
}

void RenderSystem::SubmitScene()
{
    mRegistry->BeginTrace("Clear buffers");
    mRenderables.clear();
    mUploads.clear();
    mRegistry->EndTrace();

    // Pre-filter on CPU: Freya's UploadSceneInstances (sort/history/buffer copy)
    // and bindless MDI stay fast when the upload count is the frustum set, not
    // the whole world (~60k).
    const auto& projectionUniforms = mRenderer->GetCurrentProjection();
    const auto  viewProj           = projectionUniforms.projection * projectionUniforms.view;

    mRegistry->BeginTrace("Query renderables");
    mOctreeSystem->Query(Frustum(viewProj), mRenderables);
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

    // Match Freya's batch key so it can skip its internal sort.
    mRegistry->BeginTrace("Sort scene instances");
    std::ranges::sort(mUploads, [this](const fra::SceneInstanceUpload& a, const fra::SceneInstanceUpload& b) {
        const auto& meshA = mMeshPool->GetMesh(a.meshId);
        const auto& meshB = mMeshPool->GetMesh(b.meshId);
        if (meshA.vertexBufferIndex != meshB.vertexBufferIndex)
            return meshA.vertexBufferIndex < meshB.vertexBufferIndex;
        if (meshA.indexBufferIndex != meshB.indexBufferIndex)
            return meshA.indexBufferIndex < meshB.indexBufferIndex;
        if (a.meshId != b.meshId)
            return a.meshId < b.meshId;
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
