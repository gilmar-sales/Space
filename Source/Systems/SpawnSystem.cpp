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

SpawnSystem::SpawnSystem(const std::shared_ptr<fr::Scene>&        scene,
                         const std::shared_ptr<fra::MeshPool>&    meshPool,
                         const std::shared_ptr<fra::TexturePool>& texturePool) :
    System(scene), mMeshPool(meshPool), mTexturePool(texturePool)
{
    mBlankTexture = mTexturePool->CreateTextureFromFile(
        "./Resources/Textures/blank_texture.png");

    mPlayerShipModel =
        mMeshPool->CreateMeshFromFile("./Resources/Models/player_ship.fbx");
    mPlayerShipTexture = mTexturePool->CreateTextureFromFile(
        "./Resources/Textures/player_ship.jpg");

    mEnemyShipModel =
        mMeshPool->CreateMeshFromFile("./Resources/Models/enemy_ship.fbx");
    mEnemyShipTexture = mTexturePool->CreateTextureFromFile(
        "./Resources/Textures/enemy_ship.png");

    mBlackHoleModel =
        mMeshPool->CreateMeshFromFile("./Resources/Models/black_hole.fbx");

    mMoonModel = mMeshPool->CreateMeshFromFile("./Resources/Models/moon.fbx");
    mMoonTexture =
        mTexturePool->CreateTextureFromFile("./Resources/Textures/8k_moon.jpg");

    mSunModel = mMeshPool->CreateMeshFromFile("./Resources/Models/sun.fbx");
    mSunTexture =
        mTexturePool->CreateTextureFromFile("./Resources/Textures/8k_sun.jpeg");

    mCheckpointModel =
        mMeshPool->CreateMeshFromFile("./Resources/Models/checkpoint.fbx");

    mScene->CreateArchetypeBuilder()
        .WithDefault(
            ModelComponent { .meshes = &mMoonModel, .texture = mMoonTexture })
        .WithDefault(TransformComponent {})
        .WithDefault(SphereColliderComponent {})
        .WithDefault(RigidBodyComponent {})
        .WithEntities(10)
        .ForEach<TransformComponent, SphereColliderComponent,
                 RigidBodyComponent>(
            [](auto entity, TransformComponent& transform,
               SphereColliderComponent& sphereCollider,
               RigidBodyComponent&      rigidBody) {
                transform = { .position = randomPosition(-5'000, 5'000),
                              .rotation = glm::vec3(0.0),
                              .scale    = glm::vec3(randomNumber(50, 100)) };

                sphereCollider = { .radius = transform.scale.x,
                                   .offset = glm::vec3(0) };

                rigidBody = { .mass        = transform.scale.x * 5000.0f,
                              .isKinematic = true };
            })
        .Build();

    mScene->CreateArchetypeBuilder()
        .WithDefault(ModelComponent { .meshes  = &mBlackHoleModel,
                                      .texture = mBlankTexture })
        .WithDefault(TransformComponent {
            .position = glm::vec3(0),
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

                rigidBody = { .mass = transform.scale.x * 100000.0f };
            })
        .Build();

    mScene->CreateArchetypeBuilder()
        .WithDefault(
            ModelComponent { .meshes = &mSunModel, .texture = mSunTexture })
        .WithDefault(TransformComponent { .position = glm::vec3(0),
                                          .rotation = glm::vec3(0.0),
                                          .scale    = glm::vec3(200) })
        .WithDefault(
            SphereColliderComponent { .radius = 10, .offset = glm::vec3(0) })
        .WithDefault(RigidBodyComponent {})
        .ForEach<TransformComponent, SphereColliderComponent,
                 RigidBodyComponent>(
            [](auto entity, TransformComponent& transform,
               SphereColliderComponent& sphereCollider,
               RigidBodyComponent&      rigidBody) {
                transform = { .position = randomPosition(-5'000, 5'000),
                              .rotation = glm::vec3(0.0),
                              .scale    = glm::vec3(randomNumber(50, 100)) };

                sphereCollider = { .radius = transform.scale.x,
                                   .offset = glm::vec3(0) };

                rigidBody = { .mass        = transform.scale.x * 5000.0f,
                              .isKinematic = true };
            })
        .WithEntities(10)
        .Build();

    mScene->CreateArchetypeBuilder()
        .WithDefault(ModelComponent { .meshes  = &mEnemyShipModel,
                                      .texture = mEnemyShipTexture })
        .WithDefault(TransformComponent {})
        .WithDefault(SphereColliderComponent {})
        .WithDefault(RigidBodyComponent { .mass = 100.0f })
        .WithDefault(SpaceShipControlComponent { .boost = 1000 })
        .WithEntities(10'000)
        .ForEach<TransformComponent, SphereColliderComponent>(
            [](auto entity, TransformComponent& transform,
               SphereColliderComponent& sphereCollider) {
                transform = { .position = randomPosition(-10'000, 10'000),
                              .rotation = glm::vec3(0),
                              .scale    = glm::vec3(3) };

                sphereCollider = { .radius = transform.scale.x,
                                   .offset = glm::vec3(0) };
            })
        .Build();

    mScene->CreateArchetypeBuilder()
        .WithDefault(PlayerComponent { .hitPoints = 1000 })
        .WithDefault(ModelComponent { .meshes  = &mPlayerShipModel,
                                      .texture = mPlayerShipTexture })
        .WithDefault(TransformComponent { .position = glm::vec3(0),
                                          .rotation = glm::vec3(0.0, 0, 0),
                                          .scale    = glm::vec3(3) })
        .WithDefault(
            SphereColliderComponent { .radius = 3, .offset = glm::vec3(0) })
        .WithDefault(RigidBodyComponent { .mass = 110.0f })
        .WithDefault(SpaceShipControlComponent {})
        .WithEntities(1)
        .Build();
}