#include "SpaceShipSystem.hpp"

#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"

#include <Components/SpaceShipControlComponent.hpp>

void SpaceShipSystem::Update(float deltaTime)
{
    mScene->CreateQuery()->EachAsync<TransformComponent, RigidBodyComponent, SpaceShipControlComponent>(
        [deltaTime = deltaTime](TransformComponent&        transform,
                                RigidBodyComponent&        rigidBody,
                                SpaceShipControlComponent& spaceShipControl) {
            if (spaceShipControl.boost != 0)
            {
                rigidBody.ApplyForce(transform.GetForwardDirection(),
                                     spaceShipControl.boost * spaceShipControl.boostFactor, deltaTime);
            }

            if (spaceShipControl.pitchTorque != 0.0f)
            {
                transform.Rotate(transform.GetRightDirection(), spaceShipControl.pitchTorque, deltaTime);
            }

            if (spaceShipControl.yawTorque != 0.0f)
            {
                transform.Rotate(transform.GetUpDirection(), spaceShipControl.yawTorque, deltaTime);
            }

            if (spaceShipControl.rollTorque != 0.0f)
            {
                transform.Rotate(transform.GetForwardDirection(), spaceShipControl.rollTorque, deltaTime);
            }

            if (spaceShipControl.volatileTorque)
            {
                spaceShipControl.yawTorque   = 0.0f;
                spaceShipControl.pitchTorque = 0.0f;
            }
        });
}
