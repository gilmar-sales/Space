#include "PlayerCameraSystem.hpp"

#include <Freyr/Freyr.hpp>

#include <Components/PlayerComponent.hpp>
#include <Components/TransformComponent.hpp>

void PlayerCameraSystem::PostUpdate(float deltaTime)
{
    const auto player = mScene->FindUnique<PlayerComponent>();

    const auto& transform = mScene->GetComponent<TransformComponent>(player);

    const auto cameraPosition =
        transform.position - transform.GetForwardDirection() * 28.0f -
        transform.GetUpDirection() * 15.0f;

    const auto cameraForward = glm::normalize(
        transform.position + transform.GetForwardDirection() * 20.0f -
        cameraPosition);

    auto projection = fra::ProjectionUniformBuffer {
        .view = glm::lookAt(cameraPosition, cameraPosition + cameraForward,
                            transform.GetUpDirection()),
        .projection = glm::perspective(
            glm::radians(60.0f),
            static_cast<float>(mWindow->GetWidth()) /
                static_cast<float>(mWindow->GetHeight()),
            0.1f, mRenderer->GetDrawDistance())
    };

    mRenderer->UpdateProjection(projection);
}