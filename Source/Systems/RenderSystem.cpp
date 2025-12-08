#include "RenderSystem.hpp"

#include "Components/ModelComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "InputSystem.hpp"

#include <ranges>

RenderSystem::RenderSystem(const Ref<fr::Scene>&         scene,
                           const Ref<fra::Renderer>&     renderer,
                           const Ref<fra::Window>&       window,
                           const Ref<fra::MeshPool>&     meshPool,
                           const Ref<fra::MaterialPool>& materialPool,
                           const Ref<OctreeSystem>&      octreeSystem) :
    System(scene), mRenderer(renderer), mWindow(window), mMeshPool(meshPool), mMaterialPool(materialPool),
    mOctreeSystem(octreeSystem), mMatrices({}), mRenderables({}), mInstanceMatrixBuffers({})
{
    mPlayer = mScene->FindUnique<PlayerComponent>();

    mRenderables.resize(mRenderer->GetFrameCount());
    mMatrices.resize(mRenderer->GetFrameCount());
    mInstanceMatrixBuffers.resize(mRenderer->GetFrameCount());

    for (int frameIndex = 0; frameIndex < mRenderer->GetFrameCount(); ++frameIndex)
    {
        mRenderables[frameIndex].reserve(10000);
        mMatrices[frameIndex].reserve(10000);
        mInstanceMatrixBuffers[frameIndex] =
            mRenderer->GetBufferBuilder()
                .SetData(mMatrices[frameIndex].data())
                .SetSize(sizeof(glm::mat4) * 10000)
                .SetUsage(fra::BufferUsage::Instance)
                .Build();
    }
}

void RenderSystem::PostUpdate(float dt)
{
    BeginFrame();

    DrawInstanced();

    EndFrame();
}

void RenderSystem::BeginFrame() const
{
    mRenderer->BeginFrame();

    mScene->TryGetComponents<TransformComponent>(mPlayer, [&](const TransformComponent& transform) {
        const auto cameraPosition =
            transform.position - transform.GetForwardDirection() * 12.0f - transform.GetUpDirection() * 5.0f;

        const auto cameraForward =
            glm::normalize(transform.position + transform.GetForwardDirection() * 1500.0f - cameraPosition);

        auto projectionUniformBuffer = fra::ProjectionUniformBuffer {
            .view       = glm::lookAt(cameraPosition, cameraPosition + cameraForward, transform.GetUpDirection()),
            .projection = glm::perspective(
                glm::radians(75.0f), static_cast<float>(mWindow->GetWidth()) / static_cast<float>(mWindow->GetHeight()),
                0.1f, mRenderer->GetDrawDistance()),
            .ambientLight = glm::vec4(glm::normalize(glm::vec3(0.0f, 3.0f, 0.0f)), 0.5f)
        };

        mRenderer->UpdateProjection(projectionUniformBuffer);
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

    auto& [view, projection, ambientLight] = mRenderer->GetCurrentProjection();

    mScene->BeginTrace("Create Frustum");
    const auto frustum = Frustum(mRenderer->CalculateProjectionMatrix(0.1f, 15000.0f) * view);
    mScene->EndTrace();

    mScene->BeginTrace("Query renderables");
    mOctreeSystem->GetOctree()->Query(frustum, renderables);
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

    renderables = std::ranges::to<std::vector>(std::views::filter(renderables, [&](auto& particle) {
        return mScene->TryGetComponents<TransformComponent>(particle.entity, [&](const TransformComponent& transform) {
            matrices.emplace_back(transform.GetModel());
        });
    }));

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
