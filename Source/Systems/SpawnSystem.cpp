#include "SpawnSystem.hpp"

#include <random>

#include "Components/ModelComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/SpaceShipControlComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"

static int randomNumber(const int min, const int max)
{
    std::random_device              r;
    std::default_random_engine      e1(r());
    std::uniform_int_distribution<> uniform_dist(min, max);

    return uniform_dist(e1);
}

static glm::vec3 randomPosition(int min, int max)
{
    return glm::vec3 { randomNumber(min, max), randomNumber(min, max),
                       randomNumber(min, max) };
}

SpawnSystem::SpawnSystem(const std::shared_ptr<fr::Scene>&     scene,
                         const std::shared_ptr<fra::MeshPool>& meshPool) :
    System(scene), mMeshPool(meshPool)
{

    mXWingModel =
        mMeshPool->CreateMeshFromFile("./Resources/Models/x-wing.fbx");
    mRedShipModel = mMeshPool->CreateMeshFromFile(
        "./Resources/Models/cartoon_spaceship_red.fbx");
    mBlueShipModel = mMeshPool->CreateMeshFromFile(
        "./Resources/Models/cartoon_spaceship_blue.fbx");
    mMoonModel = mMeshPool->CreateMeshFromFile("./Resources/Models/moon.fbx");
    mBlackHoleModel =
        mMeshPool->CreateMeshFromFile("./Resources/Models/black_hole.fbx");
    mCheckpointModel =
        mMeshPool->CreateMeshFromFile("./Resources/Models/checkpoint.fbx");

    mScene->CreateArchetypeBuilder()
        .WithDefault(ModelComponent { .meshes = &mBlueShipModel })
        .WithDefault(TransformComponent {})
        .WithDefault(SphereColliderComponent {})
        .WithDefault(RigidBodyComponent { .mass = 100.0f })
        .WithEntities(10'000)
        .ForEach<TransformComponent, SphereColliderComponent>(
            [](auto entity, TransformComponent& transform,
               SphereColliderComponent& sphereCollider) {
                transform = { .position = randomPosition(-80'000, 80'000),
                              .rotation = glm::vec3(0),
                              .scale    = glm::vec3(15) };

                sphereCollider = { .radius = transform.scale.x,
                                   .offset = glm::vec3(0) };
            })
        .Build();

    mScene->CreateArchetypeBuilder()
        .WithDefault(ModelComponent { .meshes = &mRedShipModel })
        .WithDefault(TransformComponent {})
        .WithDefault(SphereColliderComponent {})
        .WithDefault(RigidBodyComponent { .mass = 100.0f })
        .WithEntities(10'000)
        .ForEach<TransformComponent, SphereColliderComponent>(
            [](auto entity, TransformComponent& transform,
               SphereColliderComponent& sphereCollider) {
                transform = { .position = randomPosition(-80'000, 80'000),
                              .rotation = glm::vec3(0),
                              .scale    = glm::vec3(15) };

                sphereCollider = { .radius = transform.scale.x,
                                   .offset = glm::vec3(0) };
            })
        .Build();

    mScene->CreateArchetypeBuilder()
        .WithDefault(ModelComponent { .meshes = &mMoonModel })
        .WithDefault(TransformComponent {})
        .WithDefault(SphereColliderComponent {})
        .WithDefault(RigidBodyComponent {})
        .WithEntities(1000)
        .ForEach<TransformComponent, SphereColliderComponent,
                 RigidBodyComponent>(
            [](auto entity, TransformComponent& transform,
               SphereColliderComponent& sphereCollider,
               RigidBodyComponent&      rigidBody) {
                transform = { .position = randomPosition(-80'000, 80'000),
                              .rotation = glm::vec3(0.0),
                              .scale    = glm::vec3(randomNumber(10, 1000)) };

                sphereCollider = { .radius = transform.scale.x,
                                   .offset = glm::vec3(0) };

                rigidBody = { .mass = transform.scale.x * 1000.0f };
            })
        .Build();

    mScene->CreateArchetypeBuilder()
        .WithDefault(ModelComponent { .meshes = &mBlackHoleModel })
        .WithDefault(TransformComponent {
            .position = glm::vec3(0.0),
            .rotation = glm::vec3(0.0, 60.0, 45.0),
            .scale    = glm::vec3(10'000) })
        .WithDefault(SphereColliderComponent {})
        .WithDefault(RigidBodyComponent {})
        .WithEntities(1)
        .ForEach<TransformComponent, SphereColliderComponent,
                 RigidBodyComponent>(
            [](auto entity, TransformComponent& transform,
               SphereColliderComponent& sphereCollider,
               RigidBodyComponent&      rigidBody) {
                sphereCollider = { .radius = transform.scale.x,
                                   .offset = glm::vec3(0) };

                rigidBody = { .mass = transform.scale.x * 10000.0f };
            })
        .Build();

    mScene->CreateArchetypeBuilder()
        .WithDefault(PlayerComponent { .hitPoints = 1000 })
        .WithDefault(ModelComponent { .meshes = &mXWingModel })
        .WithDefault(TransformComponent { .position = glm::vec3(1000, 1000, 0),
                                          .rotation = glm::vec3(0.0, 0, 0),
                                          .scale    = glm::vec3(10) })
        .WithDefault(
            SphereColliderComponent { .radius = 10, .offset = glm::vec3(0) })
        .WithDefault(RigidBodyComponent { .mass = 100.0f })
        .WithDefault(SpaceShipControlComponent {})
        .WithEntities(1)
        .Build();
}