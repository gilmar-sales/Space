#include "RenderSystem.hpp"

#include <shared_mutex>

#include "Components/ModelComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "InputSystem.hpp"
#include <glm/ext/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

void RenderSystem::Start()
{
    // mCubeModel = mMeshPool->CreateMeshFromFile("C:/Models/debug_cube.obj");
}

void RenderSystem::PostUpdate(float dt)
{
    mRenderer->BeginFrame();

    auto projectionBuffer = mRenderer->GetCurrentProjection();

    auto frustum = Frustum(projectionBuffer.projection * projectionBuffer.view);

    auto renderable = std::vector<Particle*>();
    renderable.reserve(2'000);
    mManager->StartTraceProfiling("Query renderables");
    mOctreeSystem->GetOctree()->Query(frustum, renderable);
    mManager->EndTraceProfiling();

    if (renderable.empty())
    {
        mRenderer->EndFrame();
        return;
    }

    std::sort(
        renderable.begin(), renderable.end(), [this](Particle* a, Particle* b) {
            return mManager->GetComponent<ModelComponent>(a->entity).meshes <
                   mManager->GetComponent<ModelComponent>(b->entity).meshes;
        });

    auto matrices = std::vector<glm::mat4>();
    matrices.reserve(renderable.size());

    mManager->StartTraceProfiling("Calculate matrizes");
    for (const auto particle : renderable)
    {
        auto matrix = glm::rotate(glm::mat4(1),
                                  particle->transform.rotation.z,
                                  glm::vec3(0.0f, 0.0f, 1.0f));
        matrix      = glm::rotate(matrix, particle->transform.rotation.y,
                                  glm::vec3(0.0f, 1.0f, 0.0f));
        matrix      = glm::rotate(matrix, particle->transform.rotation.x,
                                  glm::vec3(1.0f, 0.0f, 0.0f));
        matrix      = glm::translate(matrix, particle->transform.position);
        matrix      = glm::scale(matrix, particle->transform.scale);
        matrices.push_back(matrix);
    }
    mManager->EndTraceProfiling();

    mInstanceMatrixBuffers =
        mRenderer->GetBufferBuilder()
            .SetData(matrices.data())
            .SetSize(sizeof(glm::mat4) * matrices.size())
            .SetUsage(fra::BufferUsage::Instance)
            .Build();

    mRenderer->BindBuffer(mInstanceMatrixBuffers);

    auto instanceDraws = std::vector<InstanceDraw>();

    mManager->StartTraceProfiling("Calculate instance sequence");
    auto                        dataIndex     = 0;
    auto                        instanceCount = 0;
    std::vector<std::uint32_t>* currentMeshes = nullptr;
    for (int i = 0; i < renderable.size(); i++)
    {
        const auto& particle = renderable[i];
        auto& model = mManager->GetComponent<ModelComponent>(particle->entity);

        if (currentMeshes && currentMeshes != model.meshes)
        {
            instanceDraws.emplace_back(dataIndex, instanceCount, currentMeshes);
            instanceCount = 0;
            dataIndex     = i;
        }

        if (i == renderable.size() - 1)
        {
            instanceDraws.emplace_back(
                dataIndex, instanceCount + 1, currentMeshes);
        }
        else
        {
            currentMeshes = model.meshes;
            instanceCount += 1;
        }

        currentMeshes = model.meshes;
    }
    mManager->EndTraceProfiling();

    mManager->StartTraceProfiling("Draw calls");
    mManager->StartTraceProfiling("Draw instance sequences");
    for (const auto& draw : instanceDraws)
    {
        for (const auto& meshId : *draw.meshes)
        {
            mMeshPool->DrawInstanced(meshId, draw.instanceCount, draw.index);
        }
    }
    mManager->EndTraceProfiling();
    // for (int i = 0; i < renderable.size(); i++)
    // {
    //     const auto& particle = renderable[i];

    //     auto& model =
    //     mManager->GetComponent<ModelComponent>(particle->entity); for (auto
    //     mesh : *model.meshes)
    //         mMeshPool->DrawInstanced(mesh, 1, i);
    // }
    mManager->EndTraceProfiling();

    // mOctreeSystem->GetOctree()->Draw(mRenderer, mMeshPool, mCubeModel);

    mManager->StartTraceProfiling("Render");
    mRenderer->EndFrame();
    mManager->EndTraceProfiling();
}
