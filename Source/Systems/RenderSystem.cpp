#include "RenderSystem.hpp"

#include <shared_mutex>

#include "Components/ModelComponent.hpp"
#include "Components/TransformComponent.hpp"

#include <glm/ext/matrix_transform.hpp>

extern std::shared_ptr<fra::MeshPool> gMeshPool;

void RenderSystem::Start()
{
}

void RenderSystem::Update(float dt)
{
}

struct InstanceElement {
    fr::Entity entity;
    glm::mat4 matrix;
    std::vector<std::uint32_t>* meshes;
};

void RenderSystem::PostUpdate(float dt)
{
    mInstanceBuffers.clear();
    auto instanceData = std::vector<glm::mat4>();
    instanceData.reserve(mManager->Count<TransformComponent, ModelComponent>());

    std::vector<std::uint32_t>* currentMeshes = nullptr;

    mManager->ForEach<TransformComponent, ModelComponent>([&](fr::Entity entity, TransformComponent& transform, ModelComponent& model)
    {
        if (currentMeshes && currentMeshes != model.meshes)
            {
            const auto instanceBuffer = mRenderer->GetBufferBuilder()
                .SetData(instanceData.data())
                .SetSize(sizeof(glm::mat4) * instanceData.size())
                .SetUsage(fra::BufferUsage::Instance)
                .Build();

            mInstanceBuffers.push_back(instanceBuffer);

            mRenderer->BindBuffer(instanceBuffer);

            for(const auto& meshId : *currentMeshes)
            {
                gMeshPool->DrawInstanced(meshId, instanceData.size());
            }

            instanceData.clear();
        }

        auto matrix = glm::mat4(1);

        matrix = glm::rotate(matrix, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::rotate(matrix, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::scale(matrix, transform.scale);
        matrix = glm::translate(matrix, transform.position);

        instanceData.push_back(matrix);
        currentMeshes = model.meshes;
    });

    if (!instanceData.empty())
    {
        const auto instanceBuffer = mRenderer->GetBufferBuilder()
            .SetData(instanceData.data())
            .SetSize(sizeof(glm::mat4) * instanceData.size())
            .SetUsage(fra::BufferUsage::Instance)
            .Build();

        mInstanceBuffers.push_back(instanceBuffer);

        mRenderer->BindBuffer(instanceBuffer);

        for(const auto& meshId : *currentMeshes)
        {
            gMeshPool->DrawInstanced(meshId, instanceData.size());
        }

        instanceData.clear();
    }
}
