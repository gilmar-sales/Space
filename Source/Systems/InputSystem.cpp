#include "InputSystem.hpp"

#include <Events/KeyDownEvent.hpp>
#include <Events/KeyUpEvent.hpp>
#include <Events/MouseMoveEvent.hpp>

void InputSystem::PreUpdate(float deltaTime)
{
    static SDL_bool grab           = SDL_FALSE;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                mWindow->Close();
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                mWindow->Resize(event.window.data1, event.window.data2);
                mRenderer->RebuildSwapChain();

                break;
            case SDL_EVENT_WINDOW_MINIMIZED:
                while (event.type != SDL_EVENT_WINDOW_RESTORED)
                {
                    SDL_WaitEvent(&event);
                }

                mWindow->Resize(event.window.data1, event.window.data2);
                mRenderer->RebuildSwapChain();
                break;
            case SDL_EVENT_MOUSE_MOTION:
                mManager->SendEvent<MouseMoveEvent>(
                    { .deltaX=event.motion.xrel, .deltaY=event.motion.yrel });
                break;
            case SDL_EVENT_KEY_DOWN:
                switch (event.key.scancode)
                {
                    case SDL_SCANCODE_V:
                        mRenderer->SetVSync(!mRenderer->GetVSync());
                        break;
                    case SDL_SCANCODE_P:
                        if (mRenderer->GetSamples() ==
                            vk::SampleCountFlagBits::e1)
                        {
                            mRenderer->SetSamples(vk::SampleCountFlagBits::e8);
                        }
                        else
                        {
                            mRenderer->SetSamples(vk::SampleCountFlagBits::e1);
                        }
                        break;
                    case SDL_SCANCODE_M:
                        if (grab == SDL_TRUE)
                            grab = SDL_FALSE;
                        else
                            grab = SDL_TRUE;

                        SDL_SetRelativeMouseMode(grab);
                        break;
                    default:
                        mManager->SendEvent(KeyDownEvent {.scancode = event.key.scancode});
                        break;
                }
                break;
            case SDL_EVENT_KEY_UP: 
                {
                    mManager->SendEvent<KeyUpEvent>({ .scancode = event.key.scancode });
                    break;
                }
            default:
                break;
        }
    }
}