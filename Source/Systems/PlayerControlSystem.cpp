#include "PlayerControlSystem.hpp"

#include <Components/PlayerComponent.hpp>

#include <Components/SpaceShipControlComponent.hpp>
#include <Events/KeyDownEvent.hpp>
#include <Events/KeyUpEvent.hpp>
#include <Events/MouseMoveEvent.hpp>

PlayerControlSystem::PlayerControlSystem(
    const std::shared_ptr<fr::Scene>& scene) :
    System(scene), mPlayer(), mYawTorque()
{
    mPlayer = mScene->FindUnique<PlayerComponent>();

    mScene->AddEventListener<KeyDownEvent>(
        [this](const KeyDownEvent keyDownEvent) {
            auto& spaceShipControl =
                mScene->GetComponent<SpaceShipControlComponent>(mPlayer);
            switch (keyDownEvent.scancode)
            {
                case SDL_SCANCODE_W:
                    spaceShipControl.boost =
                        100000.0f * spaceShipControl.boostFactor;
                    break;
                case SDL_SCANCODE_A:
                    spaceShipControl.rollTorque = -100;
                    break;
                case SDL_SCANCODE_D:
                    spaceShipControl.rollTorque = 100;
                    break;
                case SDL_SCANCODE_LSHIFT:
                    spaceShipControl.boostFactor = 10.0f;
                    break;
                default:
                    break;
            }
        });

    mScene->AddEventListener<KeyUpEvent>([this](const KeyUpEvent keyUpEvent) {
        auto& spaceShipControl =
            mScene->GetComponent<SpaceShipControlComponent>(mPlayer);
        switch (keyUpEvent.scancode)
        {
            case SDL_SCANCODE_W:
                spaceShipControl.boost = 0;
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_D:
                spaceShipControl.rollTorque = 0;
                break;
            case SDL_SCANCODE_LSHIFT:
                spaceShipControl.boostFactor = 1.0f;
                break;
            default:
                break;
        }
    });

    mScene->AddEventListener<MouseMoveEvent>(
        [this](const MouseMoveEvent mouseMoveEvent) {
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
