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

void SpaceApp::Startup()
{
    mManager = std::make_shared<fr::Scene>(2'100'000);

    const auto serviceCollection = mManager->GetServiceCollection();

    serviceCollection->AddSingleton<fra::Window>(mWindow);
    serviceCollection->AddSingleton<fra::Renderer>(mRenderer);
    serviceCollection->AddSingleton<fra::MeshPool>(
        mRenderer->GetMeshPoolFactory()->CreateMeshPool());

    mManager->RegisterComponent<ModelComponent>();
    mManager->RegisterComponent<TransformComponent>();
    mManager->RegisterComponent<SphereColliderComponent>();
    mManager->RegisterComponent<RigidBodyComponent>();
    mManager->RegisterComponent<PlayerComponent>();
    mManager->RegisterComponent<SpaceShipControlComponent>();

    mManager->RegisterSystem<SpawnSystem>();
    mManager->RegisterSystem<InputSystem>();
    mManager->RegisterSystem<OctreeSystem>();
    mManager->RegisterSystem<PlayerControlSystem>();
    mManager->RegisterSystem<PlayerCameraSystem>();
    mManager->RegisterSystem<CollisionSystem>();
    mManager->RegisterSystem<MovementSystem>();
    mManager->RegisterSystem<PhysicsSystem>();
    mManager->RegisterSystem<RenderSystem>();
}

void SpaceApp::Run()
{
    Startup();

    while (mWindow->IsRunning())
    {
        mWindow->Update();
        mManager->Update(mWindow->GetDeltaTime());
    }
}
