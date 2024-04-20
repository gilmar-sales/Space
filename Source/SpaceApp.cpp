#include "SpaceApp.hpp"

void SpaceApp::Startup()
{
    gMeshPool = mRenderer->GetMeshPoolFactory()->CreateMeshPool();

    mManager = std::make_shared<fr::ECSManager>(30'000);

    auto diContainer = mManager->GetDIContainer();

    diContainer->AddSingleton<fra::Window>(mWindow);
    diContainer->AddSingleton<fra::Renderer>(mRenderer);

    mManager->RegisterComponent<ModelComponent>();
    mManager->RegisterComponent<TransformComponent>();
    mManager->RegisterComponent<SphereColliderComponent>();
    mManager->RegisterComponent<RigidBodyComponent>();

    mManager->RegisterSystem<InputSystem>();
    mManager->RegisterSystem<SpawnSystem>();
    mManager->RegisterSystem<OctreeSystem>();
    mManager->RegisterSystem<CollisionSystem>();
    mManager->RegisterSystem<MovementSystem>();
    mManager->RegisterSystem<PhysicsSystem>();
    mManager->RegisterSystem<RenderSystem>();
}

void SpaceApp::Update()
{
    mManager->Update(mWindow->GetDeltaTime());
}
