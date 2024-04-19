#pragma once

#include <Core/AbstractApplication.hpp>
#include <Freyr.hpp>

#include "Components/ModelComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "Systems/CollisionSystem.hpp"
#include "Systems/InputSystem.hpp"
#include "Systems/MovementSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/SpawnSystem.hpp"

extern std::shared_ptr<fra::MeshPool> gMeshPool;

class SpaceApp : public fra::AbstractApplication
{
  public:
    void Startup() override;

    void Update() override;

  private:
    std::shared_ptr<fr::ECSManager> mManager;
};