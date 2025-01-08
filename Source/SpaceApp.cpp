#include "SpaceApp.hpp"

#include "Components/ModelComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"
#include <Components/SpaceShipControlComponent.hpp>

#include "Systems/CollisionSystem.hpp"
#include "Systems/InputSystem.hpp"
#include "Systems/MovementSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/SpawnSystem.hpp"
#include <Systems/PlayerCameraSystem.hpp>
#include <Systems/PlayerControlSystem.hpp>

void SpaceApp::StartUp()
{
    mScene = std::make_shared<fr::Scene>(2'100'000);
    mScene->StartProfiling();

    const auto serviceCollection = mScene->GetServiceCollection();

    serviceCollection->AddSingleton<fra::Window>(mWindow);
    serviceCollection->AddSingleton<fra::Renderer>(mRenderer);
    serviceCollection->AddSingleton<fra::MeshPool>(
        mRenderer->GetMeshPoolFactory()->CreateMeshPool());
    serviceCollection->AddSingleton<fra::TexturePool>(
        mRenderer->GetTexturePoolFactory()->CreateTexturePool());
    serviceCollection->AddSingleton<fra::EventManager>(mEventManager);

    mScene->RegisterComponent<ModelComponent>();
    mScene->RegisterComponent<TransformComponent>();
    mScene->RegisterComponent<SphereColliderComponent>();
    mScene->RegisterComponent<RigidBodyComponent>();
    mScene->RegisterComponent<PlayerComponent>();
    mScene->RegisterComponent<SpaceShipControlComponent>();

    mScene->RegisterSystem<SpawnSystem>();
    mScene->RegisterSystem<InputSystem>();
    mScene->RegisterSystem<OctreeSystem>();
    mScene->RegisterSystem<PlayerControlSystem>();
    mScene->RegisterSystem<PlayerCameraSystem>();
    mScene->RegisterSystem<CollisionSystem>();
    mScene->RegisterSystem<MovementSystem>();
    mScene->RegisterSystem<PhysicsSystem>();
    mScene->RegisterSystem<RenderSystem>();

    SDL_AddGamepadMappingsFromFile("./Resources/gamecontrollerdb.txt");

    auto gamepadCount = 0;
    auto gamepads     = SDL_GetGamepads(&gamepadCount);
    if (gamepadCount > 0)
    {
        auto gamepad = SDL_OpenGamepad(gamepads[1]);
        if (gamepad)
        {
            std::printf("Found gamepad %s\n", SDL_GetGamepadName(gamepad));
        }
    }
}

void SpaceApp::Update()
{
    mScene->Update(mWindow->GetDeltaTime());
}

void SpaceApp::ShutDown()
{
    mScene->EndProfiling();
}
