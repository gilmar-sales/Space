#pragma once

#include <Core/AbstractApplication.hpp>
#include <Freyr.hpp>

#include "Components/ModelComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/RigidBodyComponent.hpp"

#include "Systems/SpawnSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/MovementSystem.hpp"
#include "Systems/CollisionSystem.hpp"
#include "Systems/InputSystem.hpp"
#include "Systems/PhysicsSystem.hpp"

extern std::shared_ptr<fra::MeshPool> gMeshPool;

class SpaceApp : public fra::AbstractApplication
{
public:
    void Startup() override
    {
        gMeshPool = mRenderer->GetMeshPoolFactory()->CreateMeshPool();

        mManager = std::make_shared<fr::ECSManager>(30'000);

        mManager->RegisterComponent<ModelComponent>();
        mManager->RegisterComponent<TransformComponent>();
        mManager->RegisterComponent<SphereColliderComponent>();
        mManager->RegisterComponent<RigidBodyComponent>();
        
        auto inputSystem = mManager->RegisterSystem<InputSystem>();
        inputSystem->mRenderer = mRenderer;
        inputSystem->mWindow = mWindow;

        mManager->RegisterSystem<SpawnSystem>();
        mManager->RegisterSystem<MovementSystem>();
        mManager->RegisterSystem<PhysicsSystem>();
        mManager->RegisterSystem<CollisionSystem>()->mRenderer = mRenderer;

        auto renderSystem = mManager->RegisterSystem<RenderSystem>();
        renderSystem->mRenderer = mRenderer;
        mRenderer->SetDrawDistance(1000000.0f);
    }

    void Update() override
    {
        mManager->Update(mWindow->GetDeltaTime());
    }

private:
    std::shared_ptr<fr::ECSManager> mManager;
};