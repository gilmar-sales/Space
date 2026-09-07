#include "SpaceShipSystem.hpp"

#include "Components/RigidBodyComponent.hpp"
#include "Components/SpaceShipControlComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "glm/gtx/norm.hpp"

#include <cmath>

void SpaceShipSystem::Update(float deltaTime)
{
    const float safeDt = glm::max(deltaTime, 1.0e-6f);

    mRegistry->CreateMutation()->EachAsync(
        [deltaTime, safeDt](TransformComponent&        transform,
                            RigidBodyComponent&        rigidBody,
                            SpaceShipControlComponent& spaceShipControl) {
            if (spaceShipControl.throttle != 0.0f)
            {
                rigidBody.ApplyForce(transform.GetForwardDirection(),
                                     glm::clamp(spaceShipControl.throttle, 0.0f, 1.0f) * MaxThrust *
                                         spaceShipControl.boostFactor,
                                     deltaTime);
            }

            const glm::vec3 input = glm::clamp(
                glm::vec3(spaceShipControl.pitchInput, spaceShipControl.yawInput, spaceShipControl.rollInput),
                glm::vec3(-1.0f),
                glm::vec3(1.0f));

            const glm::vec3 targetAngularVelocity = input * MaxAngularSpeed;
            const glm::vec3 angularDelta          = targetAngularVelocity - rigidBody.angularVelocity;
            const float     maxAngularChange      = AngularAcceleration * deltaTime;

            if (glm::length2(angularDelta) > 1.0e-4f)
            {
                const float deltaLength = glm::length(angularDelta);
                rigidBody.angularVelocity +=
                    angularDelta * glm::min(1.0f, maxAngularChange / deltaLength);
            }

            if (glm::length2(input) < 1.0e-4f)
            {
                rigidBody.angularVelocity *= std::exp(-AngularDamping * deltaTime);
            }

            float angularSpeed = glm::length(rigidBody.angularVelocity);
            if (angularSpeed > MaxAngularSpeed)
            {
                rigidBody.angularVelocity *= MaxAngularSpeed / angularSpeed;
                angularSpeed = MaxAngularSpeed;
            }

            if (angularSpeed > 1.0e-4f)
            {
                const glm::vec3 worldAngularVelocity =
                    transform.GetRightDirection() * rigidBody.angularVelocity.x +
                    transform.GetUpDirection() * rigidBody.angularVelocity.y +
                    transform.GetForwardDirection() * rigidBody.angularVelocity.z;

                transform.Rotate(glm::normalize(worldAngularVelocity), angularSpeed, deltaTime);
            }

            // Mouse: queued degrees this step. Rotate by exactly that amount so
            // look sensitivity is independent of simulation rate / frame time.
            if (spaceShipControl.pitchImpulse != 0.0f)
            {
                transform.Rotate(transform.GetRightDirection(),
                                 spaceShipControl.pitchImpulse / safeDt,
                                 safeDt);
                spaceShipControl.pitchImpulse = 0.0f;
            }

            if (spaceShipControl.yawImpulse != 0.0f)
            {
                transform.Rotate(transform.GetUpDirection(),
                                 spaceShipControl.yawImpulse / safeDt,
                                 safeDt);
                spaceShipControl.yawImpulse = 0.0f;
            }
        });
}
