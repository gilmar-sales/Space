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

    mScene->AddEventListener<KeyDownEvent>([this](const KeyDownEvent
                                                      keyDownEvent) {
        switch (keyDownEvent.scancode)
        {
            case SDL_SCANCODE_W:
                mScene->GetComponent<SpaceShipControlComponent>(mPlayer).boost =
                    100000.0f;
                break;
            case SDL_SCANCODE_S:
                mScene->GetComponent<SpaceShipControlComponent>(mPlayer).boost =
                    -100000.0f;
                break;
            case SDL_SCANCODE_A:
                mScene->GetComponent<SpaceShipControlComponent>(mPlayer)
                    .rollTorque = 100;
                break;
            case SDL_SCANCODE_D:
                mScene->GetComponent<SpaceShipControlComponent>(mPlayer)
                    .rollTorque = -100;
                break;
            default:
                break;
        }
    });

    mScene->AddEventListener<KeyUpEvent>([this](const KeyUpEvent keyUpEvent) {
        switch (keyUpEvent.scancode)
        {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_S:
                mScene->GetComponent<SpaceShipControlComponent>(mPlayer).boost =
                    0;
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_D:
                mScene->GetComponent<SpaceShipControlComponent>(mPlayer)
                    .rollTorque = 0;
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
    mScene->GetComponent<SpaceShipControlComponent>(mPlayer).yawTorque   = 0;
    mScene->GetComponent<SpaceShipControlComponent>(mPlayer).pitchTorque = 0;
}
