#include "MovementSystem.hpp"

#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"

#include <Components/SpaceShipControlComponent.hpp>

void MovementSystem::FixedUpdate(float deltaTime)
{
    mScene->ForEachAsync<TransformComponent, RigidBodyComponent, SpaceShipControlComponent>(
        [manager   = mScene,
         deltaTime = deltaTime](const fr::Entity entity, TransformComponent& transform, RigidBodyComponent& rigidBody,
                                SpaceShipControlComponent& spaceShipControl) {
            if (spaceShipControl.boost != 0)
            {
                rigidBody.ApplyForce(transform.GetForwardDirection(),
                                     spaceShipControl.boost * spaceShipControl.boostFactor, deltaTime);
            }

            if (spaceShipControl.pitchTorque != 0)
            {
                transform.Rotate(transform.GetRightDirection(), spaceShipControl.pitchTorque, deltaTime);
            }

            if (spaceShipControl.yawTorque != 0)
            {
                transform.Rotate(transform.GetUpDirection(), spaceShipControl.yawTorque, deltaTime);
            }

            if (spaceShipControl.rollTorque != 0)
            {
                transform.Rotate(transform.GetForwardDirection(), spaceShipControl.rollTorque, deltaTime);
            }
        });
}
