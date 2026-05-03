#include "PlayerControlSystem.hpp"

#include "Components/LaserGunComponent.hpp"
#include <Components/PlayerComponent.hpp>
#include <Components/SpaceShipControlComponent.hpp>

PlayerControlSystem::PlayerControlSystem(const Ref<fr::Scene>& scene, const Ref<fra::EventManager>& eventManger) :
    System(scene)
{
    const auto playerOpt = mScene->CreateQuery()->FindUnique<PlayerComponent>();

    if (!playerOpt.has_value())
        return;

    auto player = playerOpt.value();

    eventManger->Subscribe<fra::MouseButtonPressedEvent>([this, player](fra::MouseButtonPressedEvent& event) {
        switch (event.button)
        {
            case fra::MouseButton::Left: {
                mScene->TryGetComponents<LaserGunComponent>(player, [](LaserGunComponent& laserGun) {
                    laserGun.triggered = true;
                });
                break;
            }
            default:
                break;
        };
    });

    eventManger->Subscribe<fra::MouseButtonReleasedEvent>([this, player](fra::MouseButtonReleasedEvent& event) {
        switch (event.button)
        {
            case fra::MouseButton::Left: {
                mScene->TryGetComponents<LaserGunComponent>(player, [](LaserGunComponent& laserGun) {
                    laserGun.triggered = false;
                });
                break;
            }
            default:
                break;
        };
    });

    eventManger->Subscribe<fra::KeyPressedEvent>([this, player](const fra::KeyPressedEvent& keyPressedEvent) {
        mScene->TryGetComponents<SpaceShipControlComponent>(player, [&](SpaceShipControlComponent& spaceShipControl) {
            switch (keyPressedEvent.key)
            {
                case fra::KeyCode::W:
                    spaceShipControl.throttle = 1.0f;
                    break;
                case fra::KeyCode::A:
                    spaceShipControl.rollTorque = -1.0f;
                    break;
                case fra::KeyCode::D:
                    spaceShipControl.rollTorque = 1.0f;
                    break;
                case fra::KeyCode::LShift:
                    spaceShipControl.boostFactor = BoostFactor;
                    break;
                default:
                    break;
            }
        });
    });

    eventManger->Subscribe<fra::KeyReleasedEvent>([this, player](const fra::KeyReleasedEvent& keyReleasedEvent) {
        mScene->TryGetComponents<SpaceShipControlComponent>(player, [&](SpaceShipControlComponent& spaceShipControl) {
            switch (keyReleasedEvent.key)
            {
                case fra::KeyCode::W:
                    spaceShipControl.throttle = 0;
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
        [this, player](const fra::GamepadButtonPressedEvent& gamepadButtonPressedEvent) {
            mScene->TryGetComponents<SpaceShipControlComponent>(
                player, [&](SpaceShipControlComponent& spaceShipControl) {
                    switch (gamepadButtonPressedEvent.button)
                    {
                        case fra::GamepadButton::GamepadButtonSouth:
                            spaceShipControl.boostFactor = BoostFactor;
                            break;
                        case fra::GamepadButton::GamepadButtonLeftShoulder:
                            mScene->TryGetComponents<LaserGunComponent>(player, [](LaserGunComponent& laserGun) {
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
        [this, player](const fra::GamepadButtonReleasedEvent& gamepadButtonReleasedEvent) {
            mScene->TryGetComponents<SpaceShipControlComponent>(
                player, [&](SpaceShipControlComponent& spaceShipControl) {
                    switch (gamepadButtonReleasedEvent.button)
                    {
                        case fra::GamepadButton::GamepadButtonSouth:
                            spaceShipControl.boostFactor = 1.0f;
                            break;
                        case fra::GamepadButton::GamepadButtonLeftShoulder:
                            mScene->TryGetComponents<LaserGunComponent>(player, [](LaserGunComponent& laserGun) {
                                laserGun.triggered = false;
                            });
                            break;
                        default: {
                            break;
                        }
                    }
                });
        });

    eventManger->Subscribe<fra::GamepadAxisMotionEvent>([this,
                                                         player](const fra::GamepadAxisMotionEvent& keyPressedEvent) {
        mScene->TryGetComponents<SpaceShipControlComponent>(player, [&](SpaceShipControlComponent& spaceShipControl) {
            spaceShipControl.volatileTorque = false;
            switch (keyPressedEvent.axis)
            {
                case fra::GamepadAxis::GamepadAxisRightTrigger:
                    spaceShipControl.throttle = static_cast<float>(1.0f * keyPressedEvent.value);
                    break;
                case fra::GamepadAxis::GamepadAxisLeftX:
                    spaceShipControl.yawTorque = static_cast<float>(keyPressedEvent.value);
                    break;
                case fra::GamepadAxis::GamepadAxisLeftY:
                    spaceShipControl.pitchTorque = -static_cast<float>(keyPressedEvent.value);
                    break;
                case fra::GamepadAxis::GamepadAxisRightX:
                    spaceShipControl.rollTorque = static_cast<float>(keyPressedEvent.value);
                    break;
                default:
                    break;
            }
        });
    });

    eventManger->Subscribe<fra::MouseMoveEvent>([this, player](const fra::MouseMoveEvent& mouseMoveEvent) {
        mScene->TryGetComponents<SpaceShipControlComponent>(player, [&](SpaceShipControlComponent& spaceShipControl) {
            spaceShipControl.yawTorque      = mouseMoveEvent.deltaX * 0.1f;
            spaceShipControl.pitchTorque    = -mouseMoveEvent.deltaY * 0.1f;
            spaceShipControl.volatileTorque = true;
        });
    });
}
