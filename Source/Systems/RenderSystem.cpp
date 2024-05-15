#include "RenderSystem.hpp"

#include <shared_mutex>

#include "Components/ModelComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "InputSystem.hpp"
#include <glm/ext/matrix_transform.hpp>

void RenderSystem::Start()
{
    // mCubeModel = mMeshPool->CreateMeshFromFile("C:/Models/debug_cube.obj");
}

void RenderSystem::PostUpdate(float dt)
{
    mRenderer->BeginFrame();

    // mManager->StartTraceProfiling("Calculate matrices");
    // auto instanceData = mManager->Map<TransformComponent, ModelComponent>(
    //     [](fr::Entity, const TransformComponent& transform,
    //        const ModelComponent&) {
    //         auto matrix = glm::rotate(glm::mat4(1), transform.rotation.z,
    //                                   glm::vec3(0.0f, 0.0f, 1.0f));
    //         matrix      = glm::rotate(matrix, transform.rotation.y,
    //                                   glm::vec3(0.0f, 1.0f, 0.0f));
    //         matrix      = glm::rotate(matrix, transform.rotation.x,
    //                                   glm::vec3(1.0f, 0.0f, 0.0f));
    //         matrix      = glm::translate(matrix, transform.position);
    //         matrix      = glm::scale(matrix, transform.scale);

    //         return matrix;
    //     });
    // mManager->EndTraceProfiling();

    // auto                        dataIndex     = instanceData.size();
    // auto                        instanceCount = 0;
    // std::vector<std::uint32_t>* currentMeshes = nullptr;

    // auto instanceDraws = std::vector<InstanceDraw>();

    // mManager->StartTraceProfiling("Calculate instance sequence");
    // mManager->ForEach<TransformComponent, ModelComponent>(
    //     [&](fr::Entity, TransformComponent&, ModelComponent& model) {
    //         if (currentMeshes && currentMeshes != model.meshes)
    //         {
    //             dataIndex -= instanceCount;

    //             instanceDraws.emplace_back(
    //                 dataIndex, instanceCount, currentMeshes);
    //             instanceCount = 0;
    //         }

    //         if (dataIndex == instanceCount + 1)
    //         {
    //             instanceDraws.emplace_back(0, instanceCount + 1,
    //             currentMeshes);
    //         }
    //         else
    //         {
    //             currentMeshes = model.meshes;
    //             instanceCount += 1;
    //         }
    //     });
    // mManager->EndTraceProfiling();

    // mInstanceMatrixBuffers =
    //     mRenderer->GetBufferBuilder()
    //         .SetData(instanceData.data())
    //         .SetSize(sizeof(glm::mat4) * instanceData.size())
    //         .SetUsage(fra::BufferUsage::Instance)
    //         .Build();

    // mRenderer->BindBuffer(mInstanceMatrixBuffers);

    // mManager->StartTraceProfiling("Draw instance sequences");
    // for (const auto& draw : instanceDraws)
    // {
    //     for (const auto& meshId : *draw.meshes)
    //     {
    //         mMeshPool->DrawInstanced(meshId, draw.instanceCount, draw.index);
    //     }
    // }
    // mManager->EndTraceProfiling();

    static float rotation = 0.0f;
    auto matrix = glm::rotate(glm::mat4(1), rotation, glm::vec3(0, 1, 0));

    auto direction = glm::vec3(glm::vec4(1, 0, 0, 0) * matrix);

    // rotation += 1.f * dt;

    auto frustum =
        Frustum { .apex      = glm::vec3(0),
                  .direction = direction,
                  .nearPlane = 0.01f,
                  .farPlane  = 10.f,
                  .fovAngle  = glm::radians(60.0f) };

    auto renderable = std::vector<Particle*>();
    mOctreeSystem->GetOctree()->Query(frustum, renderable);

    std::cout << "Renderables: " << renderable.size() << "\n";

    auto matrices = std::vector<glm::mat4>();
    matrices.reserve(renderable.size());

    for (const auto particle : renderable)
    {
        auto matrix = glm::rotate(glm::mat4(1), particle->transform.rotation.z,
                                  glm::vec3(0.0f, 0.0f, 1.0f));
        matrix      = glm::rotate(matrix, particle->transform.rotation.y,
                                  glm::vec3(0.0f, 1.0f, 0.0f));
        matrix      = glm::rotate(matrix, particle->transform.rotation.x,
                                  glm::vec3(1.0f, 0.0f, 0.0f));
        matrix      = glm::translate(matrix, particle->transform.position);
        matrix      = glm::scale(matrix, particle->transform.scale);
        matrices.push_back(matrix);
    }

    mInstanceMatrixBuffers =
        mRenderer->GetBufferBuilder()
            .SetData(matrices.data())
            .SetSize(sizeof(glm::mat4) * matrices.size())
            .SetUsage(fra::BufferUsage::Instance)
            .Build();

    for (int i = 0; i < renderable.size(); i++)
    {
        const auto& particle = renderable[i];
        mRenderer->BindBuffer(mInstanceMatrixBuffers);

        auto& model = mManager->GetComponent<ModelComponent>(particle->entity);
        for (auto mesh : *model.meshes)
            mMeshPool->DrawInstanced(mesh, 1, i);
    }

    // mOctreeSystem->GetOctree()->Draw(mRenderer, mMeshPool, mCubeModel);

    mManager->StartTraceProfiling("Render");
    mRenderer->EndFrame();
    mManager->EndTraceProfiling();
}
