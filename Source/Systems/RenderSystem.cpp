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
    mTexturePool(texturePool), mOctreeSystem(octreeSystem), mMatrices({}),
    mRenderables({})
{
    mRenderables.reserve(1024);
    mMatrices.reserve(1024);

    mInstanceMatrixBuffers =
        mRenderer->GetBufferBuilder()
            .SetData(mMatrices.data())
            .SetSize(sizeof(glm::mat4) * mMatrices.capacity())
            .SetUsage(fra::BufferUsage::Instance)
            .Build();
}

void RenderSystem::PostUpdate(float dt)
{
    mScene->StartTraceProfiling("Clear Buffers");
    mRenderables.clear();
    mMatrices.clear();
    mScene->EndTraceProfiling();

    mScene->StartTraceProfiling("Begin Frame");
    mRenderer->BeginFrame();
    mScene->EndTraceProfiling();

    auto& [view, projection, ambientLight] = mRenderer->GetCurrentProjection();

    mScene->StartTraceProfiling("Create Frustum");
    const auto frustum =
        Frustum(mRenderer->CalculateProjectionMatrix(0.1f, 5000.0f) * view);
    mScene->EndTraceProfiling();

    mScene->StartTraceProfiling("Query renderables");
    mOctreeSystem->GetOctree()->Query(frustum, mRenderables);
    mScene->EndTraceProfiling();

    if (mRenderables.empty())
    {
        mRenderer->EndFrame();
        return;
    }

    mScene->StartTraceProfiling("Sort Renderables");
    std::ranges::sort(
        mRenderables,
        [this](const Particle* a, const Particle* b) {
            return mScene->GetComponent<ModelComponent>(a->entity).meshes <
                       mScene->GetComponent<ModelComponent>(b->entity).meshes &&
                   mScene->GetComponent<ModelComponent>(a->entity).texture <
                       mScene->GetComponent<ModelComponent>(b->entity).texture;
        });
    mScene->EndTraceProfiling();

    if (mRenderables.size() > mMatrices.capacity())
        mMatrices.reserve(mRenderables.size());

    FREYR_PROFILING_BEGIN("USER",
                          "Calculate matrizes",
                          perfetto::Track(2),
                          "rendeable count",
                          mRenderables.size());
    // mScene->StartTraceProfiling("Calculate matrizes");
    for (const auto particle : mRenderables)
    {
        mMatrices.emplace_back(particle->transform->GetModel());
    }
    mScene->EndTraceProfiling();

    if (mInstanceMatrixBuffers->GetSize() < mMatrices.capacity())
        mInstanceMatrixBuffers =
            mRenderer->GetBufferBuilder()
                .SetData(mMatrices.data())
                .SetSize(sizeof(glm::mat4) * mMatrices.capacity())
                .SetUsage(fra::BufferUsage::Instance)
                .Build();

    mInstanceMatrixBuffers->Copy(mMatrices.data(),
                                 sizeof(glm::mat4) * mMatrices.size());

    mRenderer->BindBuffer(mInstanceMatrixBuffers);

    auto instanceDraws = std::vector<InstanceDraw>();

    mScene->StartTraceProfiling("Calculate instance sequence");
    auto                        dataIndex      = 0;
    auto                        instanceCount  = 0;
    std::vector<std::uint32_t>* currentMeshes  = nullptr;
    std::uint32_t               currentTexture = 0;
    for (int i = 0; i < mRenderables.size(); i++)
    {
        const auto& particle = mRenderables[i];
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

        if (i == mRenderables.size() - 1)
        {
            if (!currentMeshes)
            {
                currentMeshes  = model.meshes;
                currentTexture = model.texture;
            }

            instanceDraws.emplace_back(dataIndex,
                                       instanceCount + 1,
                                       currentMeshes,
                                       currentTexture);
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
