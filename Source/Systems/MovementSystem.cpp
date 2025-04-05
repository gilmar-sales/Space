#include "MovementSystem.hpp"

#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "Events/ApplyForceEvent.hpp"
#include <Components/SpaceShipControlComponent.hpp>
#include <Events/ApplyTorqueEvent.hpp>

void MovementSystem::FixedUpdate(float deltaTime)
{
    mScene->ForEachAsync<TransformComponent, RigidBodyComponent,
                         SpaceShipControlComponent>(
        [manager = mScene, deltaTime = deltaTime](
            const fr::Entity entity, const TransformComponent& transform,
            RigidBodyComponent&              rigidBody,
            const SpaceShipControlComponent& spaceShipControl) {
            if (spaceShipControl.boost != 0)
            {
                manager->SendEvent(ApplyForceEvent {
                    .target    = entity,
                    .direction = transform.GetForwardDirection(),
                    .magnetiude =
                        spaceShipControl.boost * spaceShipControl.boostFactor,
                    .deltaTime = deltaTime });
            }

            if (spaceShipControl.pitchTorque != 0)
            {
                manager->SendEvent<ApplyTorqueEvent>(ApplyTorqueEvent {
                    .target     = entity,
                    .axis       = transform.GetRightDirection(),
                    .magnetiude = spaceShipControl.pitchTorque,
                    .deltaTime  = deltaTime,
                });
            }

            if (spaceShipControl.yawTorque != 0)
            {
                manager->SendEvent<ApplyTorqueEvent>(ApplyTorqueEvent {
                    .target     = entity,
                    .axis       = transform.GetUpDirection(),
                    .magnetiude = spaceShipControl.yawTorque,
                    .deltaTime  = deltaTime,
                });
            }

            if (spaceShipControl.rollTorque != 0)
            {
                manager->SendEvent<ApplyTorqueEvent>(ApplyTorqueEvent {
                    .target     = entity,
                    .axis       = transform.GetForwardDirection(),
                    .magnetiude = spaceShipControl.rollTorque,
                    .deltaTime  = deltaTime,
                });
            }
        });
}
