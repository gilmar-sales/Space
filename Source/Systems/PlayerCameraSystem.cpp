#include "PlayerCameraSystem.hpp"

#include <Freyr/Freyr.hpp>

#include <Components/TransformComponent.hpp>

void PlayerCameraSystem::PostUpdate(float deltaTime)
{
    auto& transform = mScene->GetComponent<TransformComponent>(mPlayer);

    const auto cameraPosition =
        transform.position - transform.GetForwardDirection() * 30.0f -
        transform.GetUpDirection() * 13.0f;

    const auto cameraForward = glm::normalize(
        transform.position + transform.GetForwardDirection() * 15.0f -
        cameraPosition);

    auto projection = fra::ProjectionUniformBuffer {
        .view = glm::lookAt(cameraPosition, cameraPosition + cameraForward,
                            transform.GetUpDirection()),
        .projection = glm::perspective(
            glm::radians(75.0f),
            static_cast<float>(mWindow->GetWidth()) /
                static_cast<float>(mWindow->GetHeight()),
            0.1f, mRenderer->GetDrawDistance())
    };

    mRenderer->UpdateProjection(projection);
    ;
}