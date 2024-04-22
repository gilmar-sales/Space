#include "InputSystem.hpp"

#include <Core/UniformBuffer.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

void InputSystem::PreUpdate(float deltaTime)
{
    static auto cameraRotation = glm::vec3(0.0f, 0.0f, 0.0f);

    auto cameraMatrix = glm::mat4(1.0f);
    cameraMatrix      = glm::rotate(cameraMatrix, glm::radians(cameraRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    cameraMatrix      = glm::rotate(cameraMatrix, glm::radians(cameraRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    cameraMatrix      = glm::rotate(cameraMatrix, glm::radians(cameraRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    static auto cameraPosition = glm::vec3(0.0f, 0.0f, -10.0f);
    auto        cameraForward  = glm::vec3(glm::vec4(0.0f, 0.0f, 1.0f, 0.0) * cameraMatrix);
    auto        cameraRight    = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraForward));
    auto        cameraUp       = glm::normalize(glm::cross(cameraForward, cameraRight));

    static auto     cameraVelocity = glm::vec2(0.0f, 0.0f);
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
            case SDL_EVENT_KEY_DOWN:
                switch (event.key.keysym.scancode)
                {
                    case SDL_SCANCODE_V:
                        mRenderer->SetVSync(!mRenderer->GetVSync());
                        break;
                    case SDL_SCANCODE_P:
                        if (mRenderer->GetSamples() == vk::SampleCountFlagBits::e1)
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
                    case SDL_SCANCODE_W:
                        cameraVelocity.y = 1.0f;
                        break;
                    case SDL_SCANCODE_S:
                        cameraVelocity.y = -1.0f;
                        break;
                    case SDL_SCANCODE_D:
                        cameraVelocity.x = -1.0f;
                        break;
                    case SDL_SCANCODE_A:
                        cameraVelocity.x = 1.0f;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_EVENT_KEY_UP:
                switch (event.key.keysym.scancode)
                {
                    case SDL_SCANCODE_W:
                        cameraVelocity.y = 0.0f;
                        break;
                    case SDL_SCANCODE_S:
                        cameraVelocity.y = 0.0f;
                        break;
                    case SDL_SCANCODE_D:
                        cameraVelocity.x = 0.0f;
                        break;
                    case SDL_SCANCODE_A:
                        cameraVelocity.x = 0.0f;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_EVENT_MOUSE_MOTION:
                cameraRotation.y += event.motion.xrel * 10.0f * deltaTime;
                cameraRotation.x += event.motion.yrel * 10.0f * deltaTime;
                break;
            default:
                break;
        }
    }

    cameraPosition += cameraForward * cameraVelocity.y * 100.0f * deltaTime;
    cameraPosition += cameraRight * cameraVelocity.x * 100.0f * deltaTime;

    auto projection = fra::ProjectionUniformBuffer {
        .view       = glm::lookAt(cameraPosition, cameraPosition + cameraForward, cameraUp),
        .projection = glm::perspective(glm::radians(45.0f), mWindow->GetWidth() / (float) mWindow->GetHeight(), 0.001f,
                                       mRenderer->GetDrawDistance())
    };

    mRenderer->UpdateProjection(projection);
}