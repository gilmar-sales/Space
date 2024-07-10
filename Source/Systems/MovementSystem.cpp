#include "MovementSystem.hpp"

#include <glm/ext/matrix_transform.hpp>

#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "Events/ApplyForceEvent.hpp"
#include "Events/CollisionEvent.hpp"
#include <Components/SpaceShipControlComponent.hpp>
#include <Events/ApplyTorqueEvent.hpp>

void MovementSystem::Update(float deltaTime)
{
    mManager->ForEachAsync<TransformComponent, RigidBodyComponent,
                           SpaceShipControlComponent>(
        [manager = mManager, deltaTime = deltaTime](
            fr::Entity entity, TransformComponent& transform,
            RigidBodyComponent& rigidBody, SpaceShipControlComponent& spaceShipControl) {
            if (spaceShipControl.boost != 0)
            {
                manager->SendEvent(ApplyForceEvent {
                    .target     = entity,
                    .direction  = transform.GetForwardDirection(),
                    .magnetiude = spaceShipControl.boost,
                    .deltaTime  = deltaTime });
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
                    .target = entity,
                    .axis = transform.GetForwardDirection(),
                    .magnetiude = spaceShipControl.rollTorque,
                    .deltaTime = deltaTime,});
            }
        });
}
