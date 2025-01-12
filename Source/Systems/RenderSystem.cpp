#include "RenderSystem.hpp"

#include "Components/ModelComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "InputSystem.hpp"

RenderSystem::RenderSystem(const std::shared_ptr<fr::Scene>&        scene,
                           const std::shared_ptr<fra::Renderer>&    renderer,
                           const std::shared_ptr<fra::MeshPool>&    meshPool,
                           const std::shared_ptr<fra::TexturePool>& texturePool,
                           const std::shared_ptr<OctreeSystem>& octreeSystem) :
    System(scene), mRenderer(renderer), mMeshPool(meshPool),
    mTexturePool(texturePool), mOctreeSystem(octreeSystem)
{
}

void RenderSystem::PostUpdate(float dt)
{
    mRenderer->BeginFrame();

    auto& [view, projection, ambientLight] = mRenderer->GetCurrentProjection();

    const auto frustum =
        Frustum(mRenderer->CalculateProjectionMatrix(0.1f, 5000.0f) * view);

    auto renderables = std::vector<Particle*>();
    renderables.reserve(10'000);
    mScene->StartTraceProfiling("Query renderables");
    mOctreeSystem->GetOctree()->Query(frustum, renderables);
    mScene->EndTraceProfiling();

    if (renderables.empty())
    {
        mRenderer->EndFrame();
        return;
    }

    std::ranges::sort(
        renderables,
        [this](const Particle* a, const Particle* b) {
            return mScene->GetComponent<ModelComponent>(a->entity).meshes <
                       mScene->GetComponent<ModelComponent>(b->entity).meshes &&
                   mScene->GetComponent<ModelComponent>(a->entity).texture <
                       mScene->GetComponent<ModelComponent>(b->entity).texture;
        });

    auto matrices = std::vector<glm::mat4>();
    matrices.reserve(renderables.size());

    mScene->StartTraceProfiling("Calculate matrizes");
    for (const auto particle : renderables)
    {
        matrices.emplace_back(particle->transform->GetModel());
    }
    mScene->EndTraceProfiling();

    mInstanceMatrixBuffers =
        mRenderer->GetBufferBuilder()
            .SetData(matrices.data())
            .SetSize(sizeof(glm::mat4) * matrices.size())
            .SetUsage(fra::BufferUsage::Instance)
            .Build();

    mRenderer->BindBuffer(mInstanceMatrixBuffers);

    auto instanceDraws = std::vector<InstanceDraw>();

    mScene->StartTraceProfiling("Calculate instance sequence");
    auto                        dataIndex      = 0;
    auto                        instanceCount  = 0;
    std::vector<std::uint32_t>* currentMeshes  = nullptr;
    std::uint32_t               currentTexture = 0;
    for (int i = 0; i < renderables.size(); i++)
    {
        const auto& particle = renderables[i];
        const auto& model =
            mScene->GetComponent<ModelComponent>(particle->entity);

        if (currentMeshes &&
            (currentMeshes != model.meshes || currentTexture != model.texture))
        {
            instanceDraws.emplace_back(dataIndex,
                                       instanceCount,
                                       currentMeshes,
                                       currentTexture);
            currentMeshes = nullptr;
            instanceCount = 0;
            dataIndex     = i;
        }

        if (i == renderables.size() - 1)
        {
            if (!currentMeshes)
            {
                currentMeshes  = model.meshes;
                currentTexture = model.texture;
            }

            instanceDraws.emplace_back(
                dataIndex, instanceCount + 1, currentMeshes, currentTexture);
        }
        else
        {
            currentMeshes  = model.meshes;
            currentTexture = model.texture;
            instanceCount += 1;
        }
    }
    mScene->EndTraceProfiling();

    mScene->StartTraceProfiling("Draw instance sequences");
    for (const auto& draw : instanceDraws)
    {
        mTexturePool->Bind(draw.texture);

        if (draw.meshes != nullptr)
            for (const auto& meshId : *draw.meshes)
            {
                mMeshPool->DrawInstanced(meshId,
                                         draw.instanceCount,
                                         draw.index);
            }
    }
    mScene->EndTraceProfiling();

    mScene->StartTraceProfiling("Render");
    mRenderer->EndFrame();
    mScene->EndTraceProfiling();
}
