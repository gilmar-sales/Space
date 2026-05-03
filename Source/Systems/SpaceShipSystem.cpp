#include "SpaceShipSystem.hpp"

#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "glm/gtx/norm.hpp"

#include <Components/SpaceShipControlComponent.hpp>

void SpaceShipSystem::Update(float deltaTime)
{
    mScene->CreateQuery()->EachAsync<TransformComponent, RigidBodyComponent, SpaceShipControlComponent>(
        [deltaTime = deltaTime](TransformComponent&        transform,
                                RigidBodyComponent&        rigidBody,
                                SpaceShipControlComponent& spaceShipControl) {
            if (spaceShipControl.throttle != 0)
            {
                rigidBody.ApplyForce(transform.GetForwardDirection(),
                                     spaceShipControl.throttle * MaxThrust * spaceShipControl.boostFactor, deltaTime);
            }

            glm::vec3 localAngularVelocity(spaceShipControl.pitchTorque,
                                           spaceShipControl.yawTorque,
                                           spaceShipControl.rollTorque);

            // 2. Only rotate if we actually need to
            if (glm::length2(localAngularVelocity) > 0.0001f)
            {
                // 3. Convert local angular velocity to world space
                glm::vec3 worldAngularVelocity =
                    (transform.GetRightDirection() * localAngularVelocity.x) +
                    (transform.GetUpDirection() * localAngularVelocity.y) +
                    (transform.GetForwardDirection() * localAngularVelocity.z);

                // 4. Apply a single rotation
                float     angle = glm::length(worldAngularVelocity) * TurnTorque;
                glm::vec3 axis  = glm::normalize(worldAngularVelocity);

                transform.Rotate(axis, angle, deltaTime);
            }

            if (spaceShipControl.volatileTorque)
            {
                spaceShipControl.yawTorque   = 0.0f;
                spaceShipControl.pitchTorque = 0.0f;
            }
        });
}
