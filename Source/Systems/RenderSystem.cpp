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
                           const skr::Arc<fra::EventManager> &eventManager) : System(registry), mRenderables({}), mMatrices({}),
                                                                         mInstanceMatrixBuffers({}),
                                                                         mRenderer(renderer), mWindow(window),
                                                                         mMaterialPool(materialPool),
                                                                         mMeshPool(meshPool),
                                                                         mOctreeSystem(octreeSystem),
                                                                         mThreadPool(taskManager),
                                                                         mEnabled(true) {
    mPlayer = mRegistry->CreateQuery()->FindUnique<PlayerComponent>();

    mRenderables.resize(mRenderer->GetFrameCount());
    mMatrices.resize(mRenderer->GetFrameCount());
    mInstanceMatrixBuffers.resize(mRenderer->GetFrameCount());

    for (int frameIndex = 0; frameIndex < mRenderer->GetFrameCount(); ++frameIndex) {
        mRenderables[frameIndex].reserve(30000);
        mMatrices[frameIndex].reserve(30000);
        mInstanceMatrixBuffers[frameIndex] =
                mRenderer->GetBufferBuilder()
                .SetData(mMatrices[frameIndex].data())
                .SetSize(sizeof(glm::mat4) * 30000)
                .SetUsage(fra::BufferUsage::Instance)
                .Build();
    }

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
    const auto currentFrameIndex = mRenderer->GetCurrentFrameIndex();

    auto &renderables = mRenderables[currentFrameIndex];
    auto &matrices = mMatrices[currentFrameIndex];
    auto &instaceMatrixBuffer = mInstanceMatrixBuffers[currentFrameIndex];

    mRegistry->BeginTrace("Clear Buffers");
    renderables.clear();
    matrices.clear();
    mRegistry->EndTrace();

    auto projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 1.0f, 20'000.0f);
    auto view = mRenderer->GetCurrentProjection().view;

    mRegistry->BeginTrace("Create Frustum");
    const auto frustum = Frustum(projection * view);
    mRegistry->EndTrace();

    mRegistry->BeginTrace("Query renderables");
    mOctreeSystem->Query(frustum, renderables);
    mRegistry->EndTrace();

    mRegistry->BeginTrace("Sort Renderables");

    std::ranges::sort(renderables, [this](const Particle &a, const Particle &b) {
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

    if (renderables.size() > matrices.capacity())
        matrices.reserve(renderables.size());

    auto instanceDraws = std::vector<InstanceDraw>();

    mRegistry->BeginTrace("Calculate instance sequence");

    auto result = std::ranges::remove_if(renderables, [&](auto &particle) {
        return !mRegistry->TryGetComponents<TransformComponent>(particle.entity, [&](const TransformComponent &transform) {
            matrices.emplace_back(transform.GetModel());
        });
    });
    renderables.erase(result.begin(), result.end());
    auto currentInstance = InstanceDraw{.index = 0, .instanceCount = 0, .meshes = nullptr, .material = 9};

    auto i = 0;
    for (auto &renderable: renderables) {
        mRegistry->TryGetComponents<ModelComponent>(renderable.entity, [&](const ModelComponent &model) {
            if (currentInstance.meshes &&
                (currentInstance.meshes != model.meshes || currentInstance.material != model.material)) {
                instanceDraws.push_back(currentInstance);

                currentInstance.meshes = nullptr;
                currentInstance.instanceCount = 0;
                currentInstance.index = i;
            }

            if (i == renderables.size() - 1) {
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

    if (matrices.empty()) {
        return;
    }

    if (instaceMatrixBuffer->GetSize() < matrices.capacity())
        instaceMatrixBuffer =
                mRenderer->GetBufferBuilder()
                .SetData(matrices.data())
                .SetSize(sizeof(glm::mat4) * matrices.size())
                .SetUsage(fra::BufferUsage::Instance)
                .Build();

    instaceMatrixBuffer->Copy(matrices.data(), sizeof(glm::mat4) * matrices.size());

    mRenderer->BindBuffer(instaceMatrixBuffer);

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
