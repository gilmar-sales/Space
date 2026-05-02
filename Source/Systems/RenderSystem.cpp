#include "RenderSystem.hpp"

#include "Components/ModelComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "InputSystem.hpp"

#include <ranges>

RenderSystem::RenderSystem(const Ref<fr::Scene>& scene, const Ref<fra::Renderer>& renderer,
                           const Ref<fra::Window>& window, const Ref<fra::MeshPool>& meshPool,
                           const Ref<fra::MaterialPool>& materialPool, const Ref<OctreeSystem>& octreeSystem,
                           const Ref<fr::ThreadPool>& taskManager, const Ref<fra::EventManager>& eventManager) :
    System(scene), mRenderables({}), mMatrices({}), mInstanceMatrixBuffers({}), mRenderer(renderer), mWindow(window),
    mMaterialPool(materialPool), mMeshPool(meshPool), mOctreeSystem(octreeSystem), mThreadPool(taskManager),
    mEnabled(true)
{
    mPlayer = mScene->CreateQuery()->FindUnique<PlayerComponent>();

    mRenderables.resize(mRenderer->GetFrameCount());
    mMatrices.resize(mRenderer->GetFrameCount());
    mInstanceMatrixBuffers.resize(mRenderer->GetFrameCount());

    for (int frameIndex = 0; frameIndex < mRenderer->GetFrameCount(); ++frameIndex)
    {
        mRenderables[frameIndex].reserve(30000);
        mMatrices[frameIndex].reserve(30000);
        mInstanceMatrixBuffers[frameIndex] =
            mRenderer->GetBufferBuilder()
                .SetData(mMatrices[frameIndex].data())
                .SetSize(sizeof(glm::mat4) * 30000)
                .SetUsage(fra::BufferUsage::Instance)
                .Build();
    }

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

    DrawInstanced();

    EndFrame();
}

void RenderSystem::BeginFrame() const
{
    mThreadPool->WaitForAllTasks();
    mRenderer->BeginFrame();

    if (mPlayer.has_value())
        mScene->TryGetComponents<TransformComponent>(mPlayer.value(), [this](const TransformComponent& transform) {
            const auto cameraPosition =
                transform.position - transform.GetForwardDirection() * 15.0f + transform.GetUpDirection() * 4.0f;

            const auto cameraForward =
                glm::normalize(transform.position + transform.GetForwardDirection() * 1500.0f - cameraPosition);

            mRenderer->UpdateCamera(cameraPosition, cameraPosition + cameraForward, transform.GetUpDirection());
        });
}

void RenderSystem::DrawInstanced()
{
    const auto currentFrameIndex = mRenderer->GetCurrentFrameIndex();

    auto& renderables         = mRenderables[currentFrameIndex];
    auto& matrices            = mMatrices[currentFrameIndex];
    auto& instaceMatrixBuffer = mInstanceMatrixBuffers[currentFrameIndex];

    mScene->BeginTrace("Clear Buffers");
    renderables.clear();
    matrices.clear();
    mScene->EndTrace();

    auto projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 1.0f, 20'000.0f);
    auto view       = mRenderer->GetCurrentProjection().view;

    mScene->BeginTrace("Create Frustum");
    const auto frustum = Frustum(projection * view);
    mScene->EndTrace();

    mScene->BeginTrace("Query renderables");
    mOctreeSystem->Query(frustum, renderables);
    mScene->EndTrace();

    mScene->BeginTrace("Sort Renderables");

    std::ranges::sort(renderables, [this](const Particle& a, const Particle& b) {
        auto greater = false;
        mScene->TryGetComponents<ModelComponent>(a.entity, [&](const ModelComponent& aModel) {
            mScene->TryGetComponents<ModelComponent>(b.entity, [&](const ModelComponent& bModel) {
                if (aModel.meshes != bModel.meshes)
                {
                    greater = aModel.meshes < bModel.meshes;
                }
                else
                {
                    greater = aModel.material < bModel.material;
                }
            });
        });
        return greater;
    });
    mScene->EndTrace();

    if (renderables.size() > matrices.capacity())
        matrices.reserve(renderables.size());

    auto instanceDraws = std::vector<InstanceDraw>();

    mScene->BeginTrace("Calculate instance sequence");

    auto result = std::ranges::remove_if(renderables, [&](auto& particle) {
        return !mScene->TryGetComponents<TransformComponent>(particle.entity, [&](const TransformComponent& transform) {
            matrices.emplace_back(transform.GetModel());
        });
    });
    renderables.erase(result.begin(), result.end());
    auto currentInstance = InstanceDraw { .index = 0, .instanceCount = 0, .meshes = nullptr, .material = 9 };

    auto i = 0;
    for (auto& renderable : renderables)
    {
        mScene->TryGetComponents<ModelComponent>(renderable.entity, [&](const ModelComponent& model) {
            if (currentInstance.meshes &&
                (currentInstance.meshes != model.meshes || currentInstance.material != model.material))
            {
                instanceDraws.push_back(currentInstance);

                currentInstance.meshes        = nullptr;
                currentInstance.instanceCount = 0;
                currentInstance.index         = i;
            }

            if (i == renderables.size() - 1)
            {
                if (!currentInstance.meshes)
                {
                    currentInstance.meshes   = model.meshes;
                    currentInstance.material = model.material;
                }

                currentInstance.instanceCount += 1;

                instanceDraws.push_back(currentInstance);
            }
            else
            {
                currentInstance.meshes   = model.meshes;
                currentInstance.material = model.material;
                currentInstance.instanceCount += 1;
            }
            i++;
        });
    }
    mScene->EndTrace();

    if (matrices.empty())
    {
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

    mScene->BeginTrace("Draw instance sequences");
    for (const auto& instanceDraw : instanceDraws)
    {
        mMaterialPool->Bind(instanceDraw.material);

        if (instanceDraw.meshes != nullptr)
            for (const auto& meshId : *instanceDraw.meshes)
            {
                mMeshPool->DrawInstanced(meshId, instanceDraw.instanceCount, instanceDraw.index);
            }
    }
    mScene->EndTrace();
}

void RenderSystem::EndFrame() const
{
    mScene->BeginTrace("Render");
    mRenderer->EndFrame();
    mScene->EndTrace();
}
