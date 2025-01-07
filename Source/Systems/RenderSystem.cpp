#include "RenderSystem.hpp"

#include "Components/ModelComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "InputSystem.hpp"

RenderSystem::RenderSystem(const std::shared_ptr<fr::Scene>&     scene,
                           const std::shared_ptr<fra::Renderer>& renderer,
                           const std::shared_ptr<fra::MeshPool>& meshPool,
                           const std::shared_ptr<OctreeSystem>&  octreeSystem) :
    System(scene), mRenderer(renderer), mMeshPool(meshPool),
    mOctreeSystem(octreeSystem)
{
    // mCubeModel = mMeshPool->CreateMeshFromFile("C:/Models/debug_cube.obj");
}

void RenderSystem::PostUpdate(float dt)
{
    mRenderer->BeginFrame();

    auto [view, projection] = mRenderer->GetCurrentProjection();

    const auto frustum = Frustum(projection * view);

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
                   mScene->GetComponent<ModelComponent>(b->entity).meshes;
        });

    auto matrices = std::vector<glm::mat4>();
    matrices.reserve(renderables.size());

    mScene->StartTraceProfiling("Calculate matrizes");
    for (const auto particle : renderables)
    {
        matrices.emplace_back(particle->transform.GetModel());
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
    auto                        dataIndex     = 0;
    auto                        instanceCount = 0;
    std::vector<std::uint32_t>* currentMeshes = nullptr;
    for (int i = 0; i < renderables.size(); i++)
    {
        const auto& particle = renderables[i];
        const auto& model =
            mScene->GetComponent<ModelComponent>(particle->entity);

        if (currentMeshes && currentMeshes != model.meshes)
        {
            instanceDraws.emplace_back(dataIndex, instanceCount, currentMeshes);
            currentMeshes = nullptr;
            instanceCount = 0;
            dataIndex     = i;
        }

        if (i == renderables.size() - 1)
        {
            if (!currentMeshes)
                currentMeshes = model.meshes;

            instanceDraws.emplace_back(dataIndex,
                                       instanceCount + 1,
                                       currentMeshes);
        }
        else
        {
            currentMeshes = model.meshes;
            instanceCount += 1;
        }
    }
    mScene->EndTraceProfiling();

    mScene->StartTraceProfiling("Draw instance sequences");
    for (const auto& draw : instanceDraws)
    {
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
