#include "PlayerControlSystem.hpp"

#include <Components/PlayerComponent.hpp>

#include <Events/KeyDownEvent.hpp>
#include <Components/SpaceShipControlComponent.hpp>
#include <Components/TransformComponent.hpp>
#include <Events/KeyUpEvent.hpp>
#include <Events/MouseMoveEvent.hpp>

void PlayerControlSystem::Start()
{
    mPlayer = mManager->FindUnique<PlayerComponent>();

    mManager->AddEventListener<KeyDownEvent>([this](const KeyDownEvent keyDownEvent) {
        switch (keyDownEvent.scancode)
        {
            case SDL_SCANCODE_W:
                mManager->GetComponent<SpaceShipControlComponent>(mPlayer)
                    .boost = 100000.0f;
                break;
            case SDL_SCANCODE_S:
                mManager->GetComponent<SpaceShipControlComponent>(mPlayer)
                    .boost = -100000.0f;
                break;
            case SDL_SCANCODE_A:
                mManager->GetComponent<SpaceShipControlComponent>(mPlayer)
                    .rollTorque = 100;
                break;
            case SDL_SCANCODE_D:
                mManager->GetComponent<SpaceShipControlComponent>(mPlayer)
                    .rollTorque = -100;
                break;
            default:
                break;
        }
    });

    mManager->AddEventListener<KeyUpEvent>([this](const KeyUpEvent keyUpEvent) {
        switch (keyUpEvent.scancode)
        {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_S:
                mManager->GetComponent<SpaceShipControlComponent>(mPlayer)
                    .boost = 0;
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_D:
                mManager->GetComponent<SpaceShipControlComponent>(mPlayer)
                    .rollTorque = 0;
                break;
            default:
                break;
        }
    });

    mManager->AddEventListener<MouseMoveEvent>(
        [this](const MouseMoveEvent mouseMoveEvent) {
        mManager->GetComponent<SpaceShipControlComponent>(mPlayer).yawTorque =
            mouseMoveEvent.deltaX;
        mManager->GetComponent<SpaceShipControlComponent>(mPlayer).pitchTorque =
            mouseMoveEvent.deltaY;
        });
}

void PlayerControlSystem::PostUpdate(float deltaTime)
{
    mManager->GetComponent<SpaceShipControlComponent>(mPlayer).yawTorque = 0;
    mManager->GetComponent<SpaceShipControlComponent>(mPlayer).pitchTorque = 0;
}
