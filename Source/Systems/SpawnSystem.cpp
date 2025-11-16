#include "SpawnSystem.hpp"

#include <random>

#include "Components/AlwaysRenderedComponent.hpp"
#include "Components/EnemyComponent.hpp"
#include "Components/LaserGunComponent.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/SpaceShipControlComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"

template <typename T>
static T randomNumber(const T min, const T max)
{
    if constexpr (std::is_floating_point_v<T>)
    {
        std::random_device                r;
        std::default_random_engine        e1(r());
        std::uniform_real_distribution<T> uniform_dist(min, max);

        return uniform_dist(e1);
    }
    else
    {
        std::random_device               r;
        std::default_random_engine       e1(r());
        std::uniform_int_distribution<T> uniform_dist(min, max);

        return uniform_dist(e1);
    }
}

static glm::vec3 randomPosition(float min, float max)
{
    return { randomNumber(min, max), randomNumber(min, max), randomNumber(min, max) };
}

SpawnSystem::SpawnSystem(const Ref<fr::Scene>&         scene,
                         const Ref<fra::MeshPool>&     meshPool,
                         const Ref<fra::TexturePool>&  texturePool,
                         const Ref<fra::MaterialPool>& materialPool) :
    System(scene), mMeshPool(meshPool), mTexturePool(texturePool), mMaterialPool(materialPool)
{
    mBlankTexture  = mTexturePool->CreateTextureFromFile("./Resources/Textures/blank_texture.png");
    mBlankMaterial = mMaterialPool->Create({ mBlankTexture, mBlankTexture, mBlankTexture });

    mPlayerShipModel    = mMeshPool->CreateMeshFromFile("./Resources/Models/player_ship.fbx");
    mPlayerShipMaterial = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/player_ship.jpg"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/player_ship_normal.jpg"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/player_ship_roughness.jpg") });

    mEnemyShipModel    = mMeshPool->CreateMeshFromFile("./Resources/Models/enemy_ship.fbx");
    mEnemyShipMaterial = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/enemy_ship.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/enemy_ship_normal.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/enemy_ship_roughness.png") });

    mMoonModel    = mMeshPool->CreateMeshFromFile("./Resources/Models/moon.fbx");
    mMoonMaterial = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/moon.jpg"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/moon_normal.png"), mBlankTexture });

    mJupiter     = mMeshPool->CreateMeshFromFile("./Resources/Models/sun.fbx");
    mSunMaterial = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/jupiter.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/jupiter_normal.png"), mBlankTexture });

    mRock1Model    = mMeshPool->CreateMeshFromFile("./Resources/Models/rock_01.fbx");
    mRock1Material = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_01.tga"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_01_normal.tga"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_01_roughness.tga") });

    mRock2Model    = mMeshPool->CreateMeshFromFile("./Resources/Models/rock_02.fbx");
    mRock2Material = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_02.tga"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_02_normal.tga"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_02_roughness.tga") });

    mCheckpointModel = mMeshPool->CreateMeshFromFile("./Resources/Models/checkpoint.fbx");

    mScene->CreateArchetypeBuilder()
        .WithComponent(ModelComponent { .meshes = &mMoonModel, .material = mMoonMaterial })
        .WithComponent(TransformComponent {})
        .WithComponent(SphereColliderComponent {})
        .WithComponent(RigidBodyComponent {})
        .ForEach<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
            [](auto entity, TransformComponent& transform, SphereColliderComponent& sphereCollider,
               RigidBodyComponent& rigidBody) {
                transform = { .position = randomPosition(-100'000, 100'000),
                              .rotation = glm::vec3(0.0),
                              .scale    = glm::vec3(randomNumber(50, 100)) };

                sphereCollider = { .radius = transform.scale.x, .offset = glm::vec3(0) };

                rigidBody = { .mass = transform.scale.x * 5000.0f, .isKinematic = true };
            })
        .WithEntities(10'000)
        .Build();

    mScene->CreateArchetypeBuilder()
        .WithComponent(ModelComponent { .meshes = &mJupiter, .material = mSunMaterial })
        .WithComponent(
            TransformComponent { .position = glm::vec3(0), .rotation = glm::vec3(0.0), .scale = glm::vec3(200) })
        .WithComponent(SphereColliderComponent { .radius = 10, .offset = glm::vec3(0) })
        .WithComponent(RigidBodyComponent {})
        .ForEach<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
            [](auto entity, TransformComponent& transform, SphereColliderComponent& sphereCollider,
               RigidBodyComponent& rigidBody) {
                transform = { .position = randomPosition(-100'000, 100'000),
                              .rotation = glm::vec3(0.0),
                              .scale    = glm::vec3(randomNumber(200.0f, 1000.0f)) };

                sphereCollider = { .radius = transform.scale.x, .offset = glm::vec3(0) };

                rigidBody = { .mass = transform.scale.x * 5000.0f, .isKinematic = true };
            })
        .WithEntities(15'000)
        .Build();

    mScene->CreateArchetypeBuilder()
        .WithComponent(ModelComponent { .meshes = &mRock1Model, .material = mRock1Material })
        .WithComponent(
            TransformComponent { .position = glm::vec3(0), .rotation = glm::vec3(0.0), .scale = glm::vec3(1) })
        .WithComponent(SphereColliderComponent { .radius = 10, .offset = glm::vec3(0) })
        .WithComponent(RigidBodyComponent {})
        .ForEach<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
            [](auto entity, TransformComponent& transform, SphereColliderComponent& sphereCollider,
               RigidBodyComponent& rigidBody) {
                transform = { .position = randomPosition(-20'000, 20'000),
                              .rotation = glm::vec3(0.0),
                              .scale    = glm::vec3(randomNumber(1.0f, 100.0f)) };

                sphereCollider = { .radius = transform.scale.x, .offset = glm::vec3(0) };

                rigidBody = { .mass = transform.scale.x * 5000.0f, .isKinematic = true };
            })
        .WithEntities(1'000)
        .Build();

    mScene->CreateArchetypeBuilder()
        .WithComponent(ModelComponent { .meshes = &mRock2Model, .material = mRock2Material })
        .WithComponent(TransformComponent { .position = glm::vec3(0),
                                            .rotation = glm::vec3(0.0),
                                            .scale    = glm::vec3(randomNumber(1.0f, 100.0f)) })
        .WithComponent(SphereColliderComponent { .radius = 10, .offset = glm::vec3(0) })
        .WithComponent(RigidBodyComponent {})
        .ForEach<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
            [](auto entity, TransformComponent& transform, SphereColliderComponent& sphereCollider,
               RigidBodyComponent& rigidBody) {
                transform = { .position = randomPosition(-20'000, 20'000),
                              .rotation = glm::vec3(0.0),
                              .scale    = glm::vec3(randomNumber(1.0f, 100.0f)) };

                sphereCollider = { .radius = transform.scale.x, .offset = glm::vec3(0) };

                rigidBody = { .mass = transform.scale.x * 5000.0f, .isKinematic = true };
            })
        .WithEntities(1'000)
        .Build();

    mScene->CreateArchetypeBuilder()
        .WithComponent(PlayerComponent { .hitPoints = 1000 })
        .WithComponent(ModelComponent { .meshes = &mPlayerShipModel, .material = mPlayerShipMaterial })
        .WithComponent(
            TransformComponent { .position = glm::vec3(0), .rotation = glm::vec3(0.0, 0, 0), .scale = glm::vec3(3) })
        .WithComponent(SphereColliderComponent { .radius = 3, .offset = glm::vec3(0) })
        .WithComponent(RigidBodyComponent { .mass = 110.0f })
        .WithComponent(SpaceShipControlComponent {})
        .WithComponent(
            LaserGunComponent { .fireRate = 0.05f, .fireTime = 0, .energyCost = 5, .energySpent = 0, .maxEnergy = 100 })
        .WithEntities(1)
        .Build();

    mScene->CreateArchetypeBuilder()
        .WithComponent(EnemyComponent { .hitPoints = 1000 })
        .WithComponent(ModelComponent { .meshes = &mEnemyShipModel, .material = mEnemyShipMaterial })
        .WithComponent(
            TransformComponent { .position = glm::vec3(0), .rotation = glm::vec3(0.0, 0, 0), .scale = glm::vec3(3) })
        .WithComponent(SphereColliderComponent { .radius = 3, .offset = glm::vec3(0) })
        .WithComponent(RigidBodyComponent { .mass = 110.0f })
        .WithComponent(SpaceShipControlComponent { .boost = Boost })
        .WithComponent(
            LaserGunComponent { .fireRate = 0.05f, .fireTime = 0, .energyCost = 5, .energySpent = 0, .maxEnergy = 80 })
        .WithEntities(5000)
        .ForEach<TransformComponent>([](auto, TransformComponent& transform) {
            transform.position = randomPosition(-1'000, 1'000);
        })
        .Build();
}