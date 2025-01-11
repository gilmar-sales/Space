#include "PlayerControlSystem.hpp"

#include <Components/PlayerComponent.hpp>

#include <Components/SpaceShipControlComponent.hpp>
#include <Events/KeyDownEvent.hpp>
#include <Events/KeyUpEvent.hpp>
#include <Events/MouseMoveEvent.hpp>

PlayerControlSystem::PlayerControlSystem(
    const std::shared_ptr<fr::Scene>&         scene,
    const std::shared_ptr<fra::EventManager>& eventManger) :
    System(scene), mPlayer(), mYawTorque()
{
    mPlayer = mScene->FindUnique<PlayerComponent>();

    eventManger->Subscribe<fra::KeyPressedEvent>(
        [this](const fra::KeyPressedEvent& keyPressedEvent) {
            auto& spaceShipControl =
                mScene->GetComponent<SpaceShipControlComponent>(mPlayer);
            switch (keyPressedEvent.key)
            {
                case fra::KeyCode::W:
                    spaceShipControl.boost = 10000.0f;
                    break;
                case fra::KeyCode::A:
                    spaceShipControl.rollTorque = -100;
                    break;
                case fra::KeyCode::D:
                    spaceShipControl.rollTorque = 100;
                    break;
                case fra::KeyCode::LSHIFT:
                    spaceShipControl.boostFactor = 10.0f;
                    break;
                default:
                    break;
            }
        });

    eventManger->Subscribe<fra::KeyReleasedEvent>(
        [this](const fra::KeyReleasedEvent& keyReleasedEvent) {
            auto& spaceShipControl =
                mScene->GetComponent<SpaceShipControlComponent>(mPlayer);
            switch (keyReleasedEvent.key)
            {
                case fra::KeyCode::W:
                    spaceShipControl.boost = 0;
                    break;
                case fra::KeyCode::A:
                case fra::KeyCode::D:
                    spaceShipControl.rollTorque = 0;
                    break;
                case fra::KeyCode::LSHIFT:
                    spaceShipControl.boostFactor = 1.0f;
                    break;
                default:
                    break;
            }
        });

    eventManger->Subscribe<fra::MouseMoveEvent>(
        [this](const fra::MouseMoveEvent& mouseMoveEvent) {
            mScene->GetComponent<SpaceShipControlComponent>(mPlayer).yawTorque =
                mouseMoveEvent.deltaX;

            mScene->GetComponent<SpaceShipControlComponent>(mPlayer)
                .pitchTorque = mouseMoveEvent.deltaY;
        });
}
void PlayerControlSystem::PostUpdate(float deltaTime)
{
    mScene->GetComponent<SpaceShipControlComponent>(mPlayer).rollTorque  = 0;
    mScene->GetComponent<SpaceShipControlComponent>(mPlayer).pitchTorque = 0;
}
