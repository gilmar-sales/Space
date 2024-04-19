#include "SpaceApp.hpp"

void SpaceApp::Startup()
{
    gMeshPool = mRenderer->GetMeshPoolFactory()->CreateMeshPool();

    mManager = std::make_shared<fr::ECSManager>(30'000);

    mManager->RegisterComponent<ModelComponent>();
    mManager->RegisterComponent<TransformComponent>();
    mManager->RegisterComponent<SphereColliderComponent>();
    mManager->RegisterComponent<RigidBodyComponent>();

    auto inputSystem       = mManager->RegisterSystem<InputSystem>();
    inputSystem->mRenderer = mRenderer;
    inputSystem->mWindow   = mWindow;

    mManager->RegisterSystem<SpawnSystem>();
    mManager->RegisterSystem<MovementSystem>();
    mManager->RegisterSystem<PhysicsSystem>();
    mManager->RegisterSystem<CollisionSystem>()->mRenderer = mRenderer;

    auto renderSystem       = mManager->RegisterSystem<RenderSystem>();
    renderSystem->mRenderer = mRenderer;
    mRenderer->SetDrawDistance(1000000.0f);
}

void SpaceApp::Update()
{
    mManager->Update(mWindow->GetDeltaTime());
}
