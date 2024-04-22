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

    return glm::vec3 { randomNumber(min, max), randomNumber(min, max), randomNumber(min, max) };
}

void SpawnSystem::Start()
{
    mRedShipModel  = mMeshPool->CreateMeshFromFile("C:/Models/cartoon_spaceship_red.obj");
    mBlueShipModel = mMeshPool->CreateMeshFromFile("C:/Models/cartoon_spaceship_blue.obj");
    mMoonModel     = mMeshPool->CreateMeshFromFile("C:/Models/moon.obj");

    for (auto i = 1; i <= 10; i++)
    {
        for (auto j = 1; j <= 10; j++)
        {
            for (auto k = 1; k <= 10; k++)
            {
                auto redShip = mManager->CreateEntity();
                mManager->AddComponent(redShip, ModelComponent { .meshes = &mRedShipModel });

                auto defaultRedTransform =
                    TransformComponent { .position = randomPosition(-1000, 1000),
                                         .rotation = glm::vec3(0),
                                         .scale    = glm::vec3(1) };
                mManager->AddComponent(redShip, defaultRedTransform);
                mManager->AddComponent(redShip, SphereColliderComponent { .radius = 1.0f, .offset = glm::vec3(0) });
                mManager->AddComponent(redShip, RigidBodyComponent { .mass = 100.0f });
            }
        }
    }

    for (auto i = 1; i <= 10; i++)
    {
        for (auto j = 1; j <= 10; j++)
        {
            for (auto k = 1; k <= 10; k++)
            {
                auto blueShip = mManager->CreateEntity();
                mManager->AddComponent(blueShip, ModelComponent { .meshes = &mBlueShipModel });

                auto defaultBlueTransform =
                    TransformComponent { .position = randomPosition(-1000, 1000),
                                         .rotation = glm::vec3(0.0),
                                         .scale    = glm::vec3(1.0, 1.0, 1.0) };
                mManager->AddComponent(blueShip, defaultBlueTransform);
                mManager->AddComponent(blueShip, SphereColliderComponent { .radius = 1.0f, .offset = glm::vec3(0) });
                mManager->AddComponent(blueShip, RigidBodyComponent { .mass = 100.0f });
            }
        }
    }

    for (auto i = 0; i < 100; i++)
    {
        auto moon = mManager->CreateEntity();
        mManager->AddComponent(moon, ModelComponent { .meshes = &mMoonModel });

        auto defaultMoonTransform =
            TransformComponent { .position = randomPosition(-100, 100),
                                 .rotation = glm::vec3(0.0),
                                 .scale    = glm::vec3(randomNumber(1, 40)) };
        mManager->AddComponent(moon, defaultMoonTransform);
        mManager->AddComponent(
            moon, SphereColliderComponent { .radius = defaultMoonTransform.scale.x, .offset = glm::vec3(0) });
        mManager->AddComponent(moon, RigidBodyComponent { .mass = defaultMoonTransform.scale.x * 100.0f });
    }
}

void SpawnSystem::Update(float dt)
{
}
