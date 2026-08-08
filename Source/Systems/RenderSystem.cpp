#include "RenderSystem.hpp"

#include "Components/ModelComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "InputSystem.hpp"

#include <ranges>

RenderSystem::RenderSystem(const skr::Arc<fr::Registry> &registry, const skr::Arc<fra::Renderer> &renderer,
                           const skr::Arc<fra::Window> &window, const skr::Arc<fra::MeshPool> &meshPool,
                           const skr::Arc<fra::MaterialPool> &materialPool, const skr::Arc<OctreeSystem> &octreeSystem,
                           const skr::Arc<fr::ThreadPool> &taskManager,
                           const skr::Arc<fra::EventManager> &eventManager) : System(registry), mRenderables({}),
                                                                         mMatrices({}),
                                                                         mRenderer(renderer), mWindow(window),
                                                                         mMaterialPool(materialPool),
                                                                         mMeshPool(meshPool),
                                                                         mOctreeSystem(octreeSystem),
                                                                         mThreadPool(taskManager),
                                                                         mEnabled(true) {
    mPlayer = mRegistry->CreateQuery()->FindUnique<PlayerComponent>();

    mRenderables.reserve(30000);
    mMatrices.reserve(30000);

    eventManager->Subscribe<fra::KeyPressedEvent>([this](const fra::KeyPressedEvent &event) {
        if (event.key != fra::KeyCode::F2)
            return;

        mEnabled = !mEnabled;
    });
}

void RenderSystem::PostUpdate(float dt) {
    if (!mEnabled)
        return;

    BeginFrame();

    DrawInstanced();

    EndFrame();
}

void RenderSystem::BeginFrame() const {
    mThreadPool->WaitForAllTasks();
    mRenderer->BeginFrame();

    if (mPlayer.has_value())
        mRegistry->TryGetComponents<TransformComponent>(mPlayer.value(), [this](const TransformComponent &transform) {
            const auto cameraPosition =
                    transform.position - transform.GetForwardDirection() * 15.0f + transform.GetUpDirection() * 4.0f;

            const auto cameraForward =
                    glm::normalize(transform.position + transform.GetForwardDirection() * 1500.0f - cameraPosition);

            mRenderer->UpdateCamera(cameraPosition, cameraPosition + cameraForward, transform.GetUpDirection());
        });
}

void RenderSystem::DrawInstanced() {
    mRegistry->BeginTrace("Clear Buffers");
    mRenderables.clear();
    mMatrices.clear();
    mRegistry->EndTrace();

    auto projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 1.0f, 20'000.0f);
    auto view = mRenderer->GetCurrentProjection().view;

    mRegistry->BeginTrace("Create Frustum");
    const auto frustum = Frustum(projection * view);
    mRegistry->EndTrace();

    mRegistry->BeginTrace("Query renderables");
    mOctreeSystem->Query(frustum, mRenderables);
    mRegistry->EndTrace();

    mRegistry->BeginTrace("Sort Renderables");

    std::ranges::sort(mRenderables, [this](const Particle &a, const Particle &b) {
        auto greater = false;
        mRegistry->TryGetComponents<ModelComponent>(a.entity, [&](const ModelComponent &aModel) {
            mRegistry->TryGetComponents<ModelComponent>(b.entity, [&](const ModelComponent &bModel) {
                if (aModel.meshes != bModel.meshes) {
                    greater = aModel.meshes < bModel.meshes;
                } else {
                    greater = aModel.material < bModel.material;
                }
            });
        });
        return greater;
    });
    mRegistry->EndTrace();

    if (mRenderables.size() > mMatrices.capacity())
        mMatrices.reserve(mRenderables.size());

    auto instanceDraws = std::vector<InstanceDraw>();

    mRegistry->BeginTrace("Calculate instance sequence");

    auto result = std::ranges::remove_if(mRenderables, [&](auto &particle) {
        return !mRegistry->TryGetComponents<TransformComponent>(particle.entity, [&](const TransformComponent &transform) {
            mMatrices.emplace_back(transform.GetModel());
        });
    });
    mRenderables.erase(result.begin(), result.end());
    auto currentInstance = InstanceDraw{.index = 0, .instanceCount = 0, .meshes = nullptr, .material = 9};

    auto i = 0;
    for (auto &renderable: mRenderables) {
        mRegistry->TryGetComponents<ModelComponent>(renderable.entity, [&](const ModelComponent &model) {
            if (currentInstance.meshes &&
                (currentInstance.meshes != model.meshes || currentInstance.material != model.material)) {
                instanceDraws.push_back(currentInstance);

                currentInstance.meshes = nullptr;
                currentInstance.instanceCount = 0;
                currentInstance.index = i;
            }

            if (i == static_cast<int>(mRenderables.size()) - 1) {
                if (!currentInstance.meshes) {
                    currentInstance.meshes = model.meshes;
                    currentInstance.material = model.material;
                }

                currentInstance.instanceCount += 1;

                instanceDraws.push_back(currentInstance);
            } else {
                currentInstance.meshes = model.meshes;
                currentInstance.material = model.material;
                currentInstance.instanceCount += 1;
            }
            i++;
        });
    }
    mRegistry->EndTrace();

    if (mMatrices.empty()) {
        return;
    }

    mRenderer->SetInstanceModels(mMatrices.data(), mMatrices.size());

    mRegistry->BeginTrace("Draw instance sequences");
    for (const auto &instanceDraw: instanceDraws) {
        if (instanceDraw.meshes != nullptr)
            for (const auto &meshId: *instanceDraw.meshes) {
                mRenderer->DrawInstanced(meshId, instanceDraw.material, instanceDraw.instanceCount, instanceDraw.index);
            }
    }
    mRegistry->EndTrace();
}

void RenderSystem::EndFrame() const {
    mRegistry->BeginTrace("Render");
    mRenderer->EndFrame();
    mRegistry->EndTrace();
}
