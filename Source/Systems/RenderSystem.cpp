#include "RenderSystem.hpp"

#include "Components/ModelComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "Components/AlwaysRenderedComponent.hpp"
#include "InputSystem.hpp"

RenderSystem::RenderSystem(const Ref<fr::Scene>&         scene,
                           const Ref<fra::Renderer>&     renderer,
                           const Ref<fra::Window>&       window,
                           const Ref<fra::MeshPool>&     meshPool,
                           const Ref<fra::MaterialPool>& materialPool,
                           const Ref<OctreeSystem>&      octreeSystem) :
    System(scene), mRenderer(renderer), mWindow(window), mMeshPool(meshPool),
    mMaterialPool(materialPool), mOctreeSystem(octreeSystem), mMatrices({}),
    mRenderables({}), mInstanceMatrixBuffers({})
{
    mPlayer = mScene->FindUnique<PlayerComponent>();

    mRenderables.resize(mRenderer->GetFrameCount());
    mMatrices.resize(mRenderer->GetFrameCount());
    mInstanceMatrixBuffers.resize(mRenderer->GetFrameCount());

    for (int frameIndex = 0; frameIndex < mRenderer->GetFrameCount();
         ++frameIndex)
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

    const auto& transform = mScene->GetComponent<TransformComponent>(mPlayer);

    const auto cameraPosition =
        transform.position - transform.GetForwardDirection() * 8.0f -
        transform.GetUpDirection() * 5.0f;

    const auto cameraForward = glm::normalize(
        transform.position + transform.GetForwardDirection() * 15.0f -
        cameraPosition);

    auto projectionUniformBuffer = fra::ProjectionUniformBuffer {
        .view = glm::lookAt(cameraPosition, cameraPosition + cameraForward,
                            transform.GetUpDirection()),
        .projection = glm::perspective(
            glm::radians(75.0f),
            static_cast<float>(mWindow->GetWidth()) /
                static_cast<float>(mWindow->GetHeight()),
            0.1f, mRenderer->GetDrawDistance()),
        .ambientLight =
            glm::vec4(glm::normalize(glm::vec3(0.0f, 3.0f, 0.0f)), 0.5f)
    };

    mRenderer->UpdateProjection(projectionUniformBuffer);
}

void RenderSystem::DrawInstanced()
{
    auto currentFrameIndex = mRenderer->GetCurrentFrameIndex();

    auto& renderables         = mRenderables[currentFrameIndex];
    auto& matrices            = mMatrices[currentFrameIndex];
    auto& instaceMatrixBuffer = mInstanceMatrixBuffers[currentFrameIndex];

    mScene->BeginTrace("Clear Buffers");
    renderables.clear();
    matrices.clear();
    mScene->EndTrace();

    auto& [view, projection, ambientLight] = mRenderer->GetCurrentProjection();

    mScene->BeginTrace("Create Frustum");
    const auto frustum =
        Frustum(mRenderer->CalculateProjectionMatrix(0.1f, 15000.0f) * view);
    mScene->EndTrace();

    mScene->BeginTrace("Query renderables");
    mOctreeSystem->GetOctree()->Query(frustum, renderables);
    mScene->EndTrace();

    mScene->BeginTrace("Sort Renderables");
    std::ranges::sort(
        renderables,
        [this](const Particle& a, const Particle& b) {
            return mScene->GetComponent<ModelComponent>(a.entity).meshes <
                       mScene->GetComponent<ModelComponent>(b.entity).meshes &&
                   mScene->GetComponent<ModelComponent>(a.entity).material <
                       mScene->GetComponent<ModelComponent>(b.entity).material;
        });
    mScene->EndTrace();

    mScene
        ->ForEach<AlwaysRenderedComponent, ModelComponent, TransformComponent>(
            [&](auto                     entity,
                AlwaysRenderedComponent& alwaysRendered,
                ModelComponent&          model,
                TransformComponent&      transform) {
                renderables.push_back(
                    { .entity = entity, .transform = &transform });
            });

    if (renderables.size() > matrices.capacity())
        matrices.reserve(renderables.size());

    mScene->BeginTrace("Calculate matrizes");

    for (const auto particle : renderables)
    {
        matrices.emplace_back(particle.transform->GetModel());
    }

    mScene->EndTrace();

    if (matrices.empty())
    {
        mScene->BeginTrace("Render");
        mRenderer->EndFrame();
        mScene->EndTrace();

        return;
    }

    if (instaceMatrixBuffer->GetSize() < matrices.capacity())
        instaceMatrixBuffer =
            mRenderer->GetBufferBuilder()
                .SetData(matrices.data())
                .SetSize(sizeof(glm::mat4) * matrices.capacity())
                .SetUsage(fra::BufferUsage::Instance)
                .Build();

    instaceMatrixBuffer->Copy(matrices.data(),
                              sizeof(glm::mat4) * matrices.size());

    mRenderer->BindBuffer(instaceMatrixBuffer);

    auto instanceDraws = std::vector<InstanceDraw>();

    mScene->BeginTrace("Calculate instance sequence");

    auto currentInstance =
        InstanceDraw { .index         = 0,
                       .instanceCount = 0,
                       .meshes        = nullptr,
                       .material      = 9 };

    for (int i = 0; i < renderables.size(); i++)
    {
        const auto& particle = renderables[i];
        const auto& model =
            mScene->GetComponent<ModelComponent>(particle.entity);

        if (currentInstance.meshes &&
            (currentInstance.meshes != model.meshes ||
             currentInstance.material != model.material))
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
    }
    mScene->EndTrace();

    mScene->BeginTrace("Draw instance sequences");
    for (const auto& instanceDraw : instanceDraws)
    {
        mMaterialPool->Bind(instanceDraw.material);

        if (instanceDraw.meshes != nullptr)
            for (const auto& meshId : *instanceDraw.meshes)
            {
                mMeshPool->DrawInstanced(meshId,
                                         instanceDraw.instanceCount,
                                         instanceDraw.index);
            }
    }
    mScene->EndTrace();
}

void RenderSystem::EndFrame() const
{
    mRenderer->EndFrame();
}
