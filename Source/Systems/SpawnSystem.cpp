#include "SpawnSystem.hpp"

#include <random>

#include "Components/EnemyComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/LaserGunComponent.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/SpaceShipControlComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"

SpawnSystem::SpawnSystem(const Ref<fr::Scene>& scene, const Ref<AssetManager>& assetManager,
                         const Ref<Random>& random) : System(scene), mAssetManager(assetManager), mRandom(random)
{
    mScene->CreateArchetypeBuilder()
        .WithComponent(HealthComponent { .hitPoints = 1000 })
        .WithComponent(
            ModelComponent { .meshes = &mAssetManager->GetMoonModel(), .material = mAssetManager->GetMoonMaterial() })
        .WithComponent(TransformComponent {})
        .WithComponent(SphereColliderComponent {})
        .WithComponent(RigidBodyComponent {})
        .ForEach<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
            [this](auto entity, TransformComponent& transform, SphereColliderComponent& sphereCollider,
                   RigidBodyComponent& rigidBody) {
                transform = { .position = mRandom->Position(-100'000, 100'000),
                              .rotation = glm::vec3(0.0),
                              .scale    = glm::vec3(mRandom->Float(50, 100)) };

                sphereCollider = { .radius = transform.scale.x, .offset = glm::vec3(0) };

                rigidBody = { .isKinematic = true, .mass = transform.scale.x * 5000.0f };
            })
        .WithEntities(30'000)
        .Build();

    mScene->CreateArchetypeBuilder()
        .WithComponent(HealthComponent { .hitPoints = 1000 })
        .WithComponent(ModelComponent { .meshes   = &mAssetManager->GetJupiterModel(),
                                        .material = mAssetManager->GetJupiterMaterial() })
        .WithComponent(
            TransformComponent { .position = glm::vec3(0), .rotation = glm::vec3(0.0), .scale = glm::vec3(200) })
        .WithComponent(SphereColliderComponent { .radius = 10, .offset = glm::vec3(0) })
        .WithComponent(RigidBodyComponent {})
        .ForEach<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
            [this](auto entity, TransformComponent& transform, SphereColliderComponent& sphereCollider,
                   RigidBodyComponent& rigidBody) {
                transform = { .position = mRandom->Position(-100'000, 100'000),
                              .rotation = glm::vec3(0.0),
                              .scale    = glm::vec3(mRandom->Float(200.0f, 1000.0f)) };

                sphereCollider = { .radius = transform.scale.x, .offset = glm::vec3(0) };

                rigidBody = { .isKinematic = true, .mass = transform.scale.x * 5000.0f };
            })
        .WithEntities(15'000)
        .Build();

    mScene->CreateArchetypeBuilder()
        .WithComponent(HealthComponent { .hitPoints = 1000 })
        .WithComponent(
            ModelComponent { .meshes = &mAssetManager->GetRock1Model(), .material = mAssetManager->GetRock1Material() })
        .WithComponent(
            TransformComponent { .position = glm::vec3(0), .rotation = glm::vec3(0.0), .scale = glm::vec3(1) })
        .WithComponent(SphereColliderComponent { .radius = 10, .offset = glm::vec3(0) })
        .WithComponent(RigidBodyComponent {})
        .ForEach<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
            [this](auto entity, TransformComponent& transform, SphereColliderComponent& sphereCollider,
                   RigidBodyComponent& rigidBody) {
                transform = { .position = mRandom->Position(-100'000, 100'000),
                              .rotation = glm::vec3(0.0),
                              .scale    = glm::vec3(mRandom->Float(1.0f, 100.0f)) };

                sphereCollider = { .radius = transform.scale.x, .offset = glm::vec3(0) };

                rigidBody = { .isKinematic = true, .mass = transform.scale.x * 5000.0f };
            })
        .WithEntities(15'000)
        .Build();

    mScene->CreateArchetypeBuilder()
        .WithComponent(HealthComponent { .hitPoints = 1000 })
        .WithComponent(
            ModelComponent { .meshes = &mAssetManager->GetRock2Model(), .material = mAssetManager->GetRock2Material() })
        .WithComponent(TransformComponent { .position = glm::vec3(0),
                                            .rotation = glm::vec3(0.0),
                                            .scale    = glm::vec3(mRandom->Float(1.0f, 100.0f)) })
        .WithComponent(SphereColliderComponent { .radius = 10, .offset = glm::vec3(0) })
        .WithComponent(RigidBodyComponent {})
        .ForEach<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
            [this](auto entity, TransformComponent& transform, SphereColliderComponent& sphereCollider,
                   RigidBodyComponent& rigidBody) {
                transform = { .position = mRandom->Position(-100'000, 100'000),
                              .rotation = glm::vec3(0.0),
                              .scale    = glm::vec3(mRandom->Float(1.0f, 100.0f)) };

                sphereCollider = { .radius = transform.scale.x, .offset = glm::vec3(0) };

                rigidBody = { .isKinematic = true, .mass = transform.scale.x * 5000.0f };
            })
        .WithEntities(10'000)
        .Build();

    mScene->CreateArchetypeBuilder()
        .WithComponent(PlayerComponent {})
        .WithComponent(HealthComponent { .hitPoints = 1000 })
        .WithComponent(ModelComponent { .meshes   = &mAssetManager->GetPlayerShipModel(),
                                        .material = mAssetManager->GetPlayerShipMaterial() })
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
        .WithComponent(EnemyComponent {})
        .WithComponent(HealthComponent { .hitPoints = 1000 })
        .WithComponent(ModelComponent { .meshes   = &mAssetManager->GetEnemyShipModel(),
                                        .material = mAssetManager->GetEnemyShipMaterial() })
        .WithComponent(
            TransformComponent { .position = glm::vec3(0), .rotation = glm::vec3(0.0, 0, 0), .scale = glm::vec3(3) })
        .WithComponent(SphereColliderComponent { .radius = 3, .offset = glm::vec3(0) })
        .WithComponent(RigidBodyComponent { .mass = 110.0f })
        .WithComponent(SpaceShipControlComponent { .boost = Boost })
        .WithComponent(
            LaserGunComponent { .fireRate = 0.05f, .fireTime = 0, .energyCost = 5, .energySpent = 0, .maxEnergy = 80 })
        .WithEntities(1'500)
        .ForEach<TransformComponent>([this](auto, TransformComponent& transform) {
            transform.position = mRandom->Position(-1'000, 1'000);
        })
        .Build();
}