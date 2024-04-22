#include "RenderSystem.hpp"

#include <shared_mutex>

#include "Components/ModelComponent.hpp"
#include "Components/TransformComponent.hpp"

#include <glm/ext/matrix_transform.hpp>

void RenderSystem::PostUpdate(float dt)
{
    mRenderer->BeginFrame();

    mInstanceBuffers.clear();

    auto instanceData = std::vector<glm::mat4>();
    instanceData.resize(mManager->Count<TransformComponent, ModelComponent>());

    mManager->ForEachParallel<TransformComponent, ModelComponent>(
        [&](fr::Entity entity, int index, TransformComponent& transform, ModelComponent& model) {
            auto& matrix = instanceData[index];

            matrix = glm::mat4(1);
            matrix = glm::rotate(matrix, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            matrix = glm::rotate(matrix, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            matrix = glm::rotate(matrix, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            matrix = glm::translate(matrix, transform.position);
            matrix = glm::scale(matrix, transform.scale);
        });

    auto                        dataIndex     = instanceData.size();
    auto                        instanceCount = 0;
    std::vector<std::uint32_t>* currentMeshes = nullptr;

    auto instanceDraws = std::vector<InstanceDraw>();

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

    for (auto& draw : instanceDraws)
    {
        const auto instanceBuffer =
            mRenderer->GetBufferBuilder()
                .SetData(&instanceData[draw.index])
                .SetSize(sizeof(glm::mat4) * draw.instanceCount)
                .SetUsage(fra::BufferUsage::Instance)
                .Build();

        mInstanceBuffers.push_back(instanceBuffer);

        mRenderer->BindBuffer(instanceBuffer);

        for (const auto& meshId : *draw.meshes)
        {
            mMeshPool->DrawInstanced(meshId, draw.instanceCount);
        }
    }

    mRenderer->EndFrame();
}
