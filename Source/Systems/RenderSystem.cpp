#include "RenderSystem.hpp"

#include <shared_mutex>

#include "Components/ModelComponent.hpp"
#include "Components/TransformComponent.hpp"

#include <glm/ext/matrix_transform.hpp>

void RenderSystem::Start()
{
    // mCubeModel = mMeshPool->CreateMeshFromFile("C:/Models/debug_cube.obj");
}

void RenderSystem::PostUpdate(float dt)
{
    mRenderer->BeginFrame();

    mManager->StartTraceProfiling("Calculate matrices");
    auto instanceData = mManager->Map<TransformComponent, ModelComponent>(
        [](fr::Entity entity, TransformComponent& transform, ModelComponent& model) {
            auto matrix = glm::rotate(glm::mat4(1), transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            matrix      = glm::rotate(matrix, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            matrix      = glm::rotate(matrix, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            matrix      = glm::translate(matrix, transform.position);
            matrix      = glm::scale(matrix, transform.scale);

            return matrix;
        });
    mManager->EndTraceProfiling();

    auto                        dataIndex     = instanceData.size();
    auto                        instanceCount = 0;
    std::vector<std::uint32_t>* currentMeshes = nullptr;

    auto instanceDraws = std::vector<InstanceDraw>();

    mManager->StartTraceProfiling("Calculate instance sequence");
    mManager->ForEach<TransformComponent, ModelComponent>(
        [&](fr::Entity entity, TransformComponent& transform, ModelComponent& model) {
            if (currentMeshes && currentMeshes != model.meshes)
            {
                dataIndex -= instanceCount;

                instanceDraws.push_back(
                    InstanceDraw { .index = dataIndex, .instanceCount = instanceCount, .meshes = currentMeshes });
                instanceCount = 0;
            }

            if (dataIndex == instanceCount + 1)
            {
                instanceDraws.push_back(
                    InstanceDraw { .index = 0, .instanceCount = instanceCount + 1, .meshes = currentMeshes });
            }
            else
            {
                currentMeshes = model.meshes;
                instanceCount += 1;
            }
        });
    mManager->EndTraceProfiling();

    mInstanceMatrixBuffers =
        mRenderer->GetBufferBuilder()
            .SetData(instanceData.data())
            .SetSize(sizeof(glm::mat4) * instanceData.size())
            .SetUsage(fra::BufferUsage::Instance)
            .Build();

    mRenderer->BindBuffer(mInstanceMatrixBuffers);

    mManager->StartTraceProfiling("Draw instance sequences");
    for (auto& draw : instanceDraws)
    {
        for (const auto& meshId : *draw.meshes)
        {
            mMeshPool->DrawInstanced(meshId, draw.instanceCount, draw.index);
        }
    }
    mManager->EndTraceProfiling();

    // mOctreeSystem->GetOctree()->Draw(mRenderer, mMeshPool, mCubeModel);

    mManager->StartTraceProfiling("Render");
    mRenderer->EndFrame();
    mManager->EndTraceProfiling();
}
