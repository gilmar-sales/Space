#include "SpaceShipSystem.hpp"

#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "glm/gtx/norm.hpp"

#include <cmath>

#include <Components/SpaceShipControlComponent.hpp>

void SpaceShipSystem::Update(float deltaTime)
{
    mRegistry->CreateMutation()->EachAsync(
        [deltaTime = deltaTime](TransformComponent&        transform,
                                RigidBodyComponent&        rigidBody,
                                SpaceShipControlComponent& spaceShipControl) {
            if (spaceShipControl.throttle != 0)
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
            const glm::vec3 angularDelta = targetAngularVelocity - rigidBody.angularVelocity;
            const float     maxAngularChange = AngularAcceleration * deltaTime;

            if (glm::length2(angularDelta) > 0.0001f)
            {
                const float deltaLength = glm::length(angularDelta);
                rigidBody.angularVelocity +=
                    angularDelta * glm::min(1.0f, maxAngularChange / deltaLength);
            }

            if (glm::length2(input) < 0.0001f)
            {
                rigidBody.angularVelocity *= std::exp(-AngularDamping * deltaTime);
            }

            const float angularSpeed = glm::length(rigidBody.angularVelocity);
            if (angularSpeed > MaxAngularSpeed)
            {
                rigidBody.angularVelocity *= MaxAngularSpeed / angularSpeed;
            }

            if (angularSpeed > 0.0001f)
            {
                const glm::vec3 worldAngularVelocity =
                    transform.GetRightDirection() * rigidBody.angularVelocity.x +
                    transform.GetUpDirection() * rigidBody.angularVelocity.y +
                    transform.GetForwardDirection() * rigidBody.angularVelocity.z;

                transform.Rotate(glm::normalize(worldAngularVelocity), glm::length(worldAngularVelocity), deltaTime);
            }

            if (spaceShipControl.volatileInput)
            {
                spaceShipControl.yawInput   = 0.0f;
                spaceShipControl.pitchInput = 0.0f;
                spaceShipControl.volatileInput = false;
            }
        });
}
