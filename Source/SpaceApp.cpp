#include "SpaceApp.hpp"

void SpaceApp::Startup()
{
    mManager = std::make_shared<fr::ECSManager>(2'100'000);

    auto diContainer = mManager->GetDIContainer();

    diContainer->AddSingleton<fra::Window>(mWindow);
    diContainer->AddSingleton<fra::Renderer>(mRenderer);
    diContainer->AddSingleton<fra::MeshPool>(
        mRenderer->GetMeshPoolFactory()->CreateMeshPool());

    mManager->RegisterComponent<ModelComponent>();
    mManager->RegisterComponent<TransformComponent>();
    mManager->RegisterComponent<SphereColliderComponent>();
    mManager->RegisterComponent<RigidBodyComponent>();
    mManager->RegisterComponent<PlayerComponent>();

    mManager->RegisterSystem<SpawnSystem>();
    mManager->RegisterSystem<InputSystem>();
    mManager->RegisterSystem<OctreeSystem>();
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
