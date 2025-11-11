#include "PlayerControlSystem.hpp"

#include "Components/LaserGunComponent.hpp"

#include <Components/PlayerComponent.hpp>

#include <Components/SpaceShipControlComponent.hpp>
#include <Events/KeyDownEvent.hpp>
#include <Events/KeyUpEvent.hpp>
#include <Events/MouseMoveEvent.hpp>


PlayerControlSystem::PlayerControlSystem(const Ref<fr::Scene>& scene, const Ref<fra::EventManager>& eventManger) :
    System(scene), mPlayer()
{
    mPlayer = mScene->FindUnique<PlayerComponent>();

    eventManger->Subscribe<fra::MouseButtonPressedEvent>([this](fra::MouseButtonPressedEvent& event) {
        switch (event.button)
        {
            case fra::MouseButton::Left: {
                mScene->TryGetComponents<LaserGunComponent>(mPlayer, [](LaserGunComponent& laserGun) {
                    laserGun.triggered = true;
                });
                break;
            }
            default:
                break;
        };
    });

    eventManger->Subscribe<fra::MouseButtonReleasedEvent>([this](fra::MouseButtonReleasedEvent& event) {
        switch (event.button)
        {
            case fra::MouseButton::Left: {
                mScene->TryGetComponents<LaserGunComponent>(mPlayer, [](LaserGunComponent& laserGun) {
                    laserGun.triggered = false;
                });
                break;
            }
            default:
                break;
        };
    });

    eventManger->Subscribe<fra::KeyPressedEvent>([this](const fra::KeyPressedEvent& keyPressedEvent) {
        mScene->TryGetComponents<SpaceShipControlComponent>(mPlayer, [&](SpaceShipControlComponent& spaceShipControl) {
            switch (keyPressedEvent.key)
            {
                case fra::KeyCode::W:
                    spaceShipControl.boost = Boost;
                    break;
                case fra::KeyCode::A:
                    spaceShipControl.rollTorque = -TurnTorque;
                    break;
                case fra::KeyCode::D:
                    spaceShipControl.rollTorque = TurnTorque;
                    break;
                case fra::KeyCode::LShift:
                    spaceShipControl.boostFactor = BoostFactor;
                    break;
                default:
                    break;
            }
        });
    });

    eventManger->Subscribe<fra::KeyReleasedEvent>([this](const fra::KeyReleasedEvent& keyReleasedEvent) {
        mScene->TryGetComponents<SpaceShipControlComponent>(mPlayer, [&](SpaceShipControlComponent& spaceShipControl) {
            switch (keyReleasedEvent.key)
            {
                case fra::KeyCode::W:
                    spaceShipControl.boost = 0;
                    break;
                case fra::KeyCode::A:
                case fra::KeyCode::D:
                    spaceShipControl.rollTorque = 0;
                    break;
                case fra::KeyCode::LShift:
                    spaceShipControl.boostFactor = 1.0f;
                    break;
                default:
                    break;
            }
        });
    });

    eventManger->Subscribe<fra::GamepadButtonPressedEvent>(
        [this](const fra::GamepadButtonPressedEvent& gamepadButtonPressedEvent) {
            mScene->TryGetComponents<SpaceShipControlComponent>(
                mPlayer, [&](SpaceShipControlComponent& spaceShipControl) {
                    switch (gamepadButtonPressedEvent.button)
                    {
                        case fra::GamepadButton::GamepadButtonSouth:
                            spaceShipControl.boostFactor = BoostFactor;
                            break;
                        case fra::GamepadButton::GamepadButtonLeftShoulder:
                            mScene->TryGetComponents<LaserGunComponent>(mPlayer, [](LaserGunComponent& laserGun) {
                                laserGun.triggered = true;
                            });
                            break;
                        default: {
                            break;
                        }
                    }
                });
        });

    eventManger->Subscribe<fra::GamepadButtonReleasedEvent>(
        [this](const fra::GamepadButtonReleasedEvent& gamepadButtonReleasedEvent) {
            mScene->TryGetComponents<SpaceShipControlComponent>(
                mPlayer, [&](SpaceShipControlComponent& spaceShipControl) {
                    switch (gamepadButtonReleasedEvent.button)
                    {
                        case fra::GamepadButton::GamepadButtonSouth:
                            spaceShipControl.boostFactor = 1.0f;
                            break;
                        case fra::GamepadButton::GamepadButtonLeftShoulder:
                            mScene->TryGetComponents<LaserGunComponent>(mPlayer, [](LaserGunComponent& laserGun) {
                                laserGun.triggered = false;
                            });
                            break;
                        default: {
                            break;
                        }
                    }
                });
        });

    eventManger->Subscribe<fra::GamepadAxisMotionEvent>([this](const fra::GamepadAxisMotionEvent& keyPressedEvent) {
        mScene->TryGetComponents<SpaceShipControlComponent>(mPlayer, [&](SpaceShipControlComponent& spaceShipControl) {
            switch (keyPressedEvent.axis)
            {
                case fra::GamepadAxis::GamepadAxisRightTrigger:
                    spaceShipControl.boost = static_cast<float>(Boost * keyPressedEvent.value);
                    break;
                case fra::GamepadAxis::GamepadAxisLeftX:
                    spaceShipControl.yawTorque = static_cast<float>(TurnTorque * keyPressedEvent.value);
                    break;
                case fra::GamepadAxis::GamepadAxisLeftY:
                    spaceShipControl.pitchTorque = static_cast<float>(TurnTorque * keyPressedEvent.value);
                    break;
                case fra::GamepadAxis::GamepadAxisRightX:
                    spaceShipControl.rollTorque = static_cast<float>(TurnTorque * keyPressedEvent.value);
                    break;
                default:
                    break;
            }
        });
    });

    eventManger->Subscribe<fra::MouseMoveEvent>([this](const fra::MouseMoveEvent& mouseMoveEvent) {
        mScene->TryGetComponents<SpaceShipControlComponent>(mPlayer, [&](SpaceShipControlComponent& spaceShipControl) {
            spaceShipControl.yawTorque   = mouseMoveEvent.deltaX * 180.0f;
            spaceShipControl.pitchTorque = mouseMoveEvent.deltaY * 180.0f;
        });
    });
}

void PlayerControlSystem::PostUpdate(float deltaTime)
{
}
