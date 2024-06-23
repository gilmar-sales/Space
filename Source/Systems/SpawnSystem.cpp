#include "SpawnSystem.hpp"

#include <random>

#include "Components/ModelComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "Components/RigidBodyComponent.hpp"
#include "Events/CollisionEvent.hpp"

int randomNumber(int min, int max)
{
    std::random_device              r;
    std::default_random_engine      e1(r());
    std::uniform_int_distribution<> uniform_dist(min, max);

    return uniform_dist(e1);
}

glm::vec3 randomPosition(int min, int max)
{

    return glm::vec3 { randomNumber(min, max), randomNumber(min, max),
                       randomNumber(min, max) };
}

void SpawnSystem::Start()
{
    mRedShipModel = mMeshPool->CreateMeshFromFile(
        "./Resources/Models/cartoon_spaceship_red.fbx");
    mBlueShipModel =
        mMeshPool->CreateMeshFromFile("./Resources/Models/x-wing.fbx");
    mMoonModel = mMeshPool->CreateMeshFromFile("./Resources/Models/moon.fbx");
    mBlackHoleModel =
        mMeshPool->CreateMeshFromFile("./Resources/Models/black_hole.fbx");
    mCheckpointModel =
        mMeshPool->CreateMeshFromFile("./Resources/Models/checkpoint.fbx");

    for (auto i = 1; i <= 10'000; i++)
    {
        auto redShip = mManager->CreateEntity();
        mManager->AddComponent(redShip,
                               ModelComponent { .meshes = &mRedShipModel });

        auto defaultRedTransform =
            TransformComponent { .position = randomPosition(-80'000, 80'000),
                                 .rotation = glm::vec3(0),
                                 .scale = glm::vec3(randomNumber(100, 150)) };
        mManager->AddComponent(redShip, defaultRedTransform);
        mManager->AddComponent(
            redShip,
            SphereColliderComponent { .radius = defaultRedTransform.scale.x,
                                      .offset = glm::vec3(0) });
        mManager->AddComponent(redShip, RigidBodyComponent { .mass = 100.0f });
    }

    for (auto i = 1; i <= 10'000; i++)
    {
        auto blueShip = mManager->CreateEntity();
        mManager->AddComponent(blueShip,
                               ModelComponent { .meshes = &mBlueShipModel });

        auto defaultBlueTransform =
            TransformComponent { .position = randomPosition(-80'000, 80'000),
                                 .rotation = glm::vec3(0.0),
                                 .scale = glm::vec3(randomNumber(100, 150)) };
        mManager->AddComponent(blueShip, defaultBlueTransform);
        mManager->AddComponent(
            blueShip,
            SphereColliderComponent { .radius = defaultBlueTransform.scale.x,
                                      .offset = glm::vec3(0) });
        mManager->AddComponent(blueShip, RigidBodyComponent { .mass = 100.0f });
    }

    for (auto i = 0; i < 100; i++)
    {
        auto moon = mManager->CreateEntity();
        mManager->AddComponent(moon, ModelComponent { .meshes = &mMoonModel });

        auto defaultMoonTransform =
            TransformComponent { .position = randomPosition(-80'000, 80'000),
                                 .rotation = glm::vec3(0.0),
                                 .scale = glm::vec3(randomNumber(100, 1000)) };
        mManager->AddComponent(moon, defaultMoonTransform);
        mManager->AddComponent(
            moon,
            SphereColliderComponent { .radius = defaultMoonTransform.scale.x,
                                      .offset = glm::vec3(0) });
        mManager->AddComponent(
            moon,
            RigidBodyComponent {
                .mass = defaultMoonTransform.scale.x * 10000.0f });
    }

    for (auto i = 0; i < 2; i++)
    {
        auto blackHole = mManager->CreateEntity();
        mManager->AddComponent(blackHole,
                               ModelComponent { .meshes = &mBlackHoleModel });

        auto defaultTransform = TransformComponent {
            .position = randomPosition(-80'000, 80'000),
            .rotation = glm::vec3(0.0),
            .scale    = glm::vec3(randomNumber(1000, 10000))
        };
        mManager->AddComponent(blackHole, defaultTransform);
        mManager->AddComponent(
            blackHole,
            SphereColliderComponent { .radius = defaultTransform.scale.x,
                                      .offset = glm::vec3(0) });
        mManager->AddComponent(
            blackHole,
            RigidBodyComponent { .mass = defaultTransform.scale.x * 10000.0f });
    }
}

void SpawnSystem::Update(float dt)
{
}
