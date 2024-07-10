#include "PlayerCameraSystem.hpp"

#include <Freyr/Freyr.hpp>

#include <Components/PlayerComponent.hpp>
#include <Components/TransformComponent.hpp>

void PlayerCameraSystem::PostUpdate(float deltaTime)
{
    auto player = mManager->FindUnique<PlayerComponent>();

    auto& transform = mManager->GetComponent<TransformComponent>(player);

    auto cameraPosition =
        transform.position - transform.GetForwardDirection() * 28.0f -
        transform.GetUpDirection() * 15.0f;

    auto cameraForward = glm::normalize(
        transform.position + transform.GetForwardDirection() * 20.0f - cameraPosition);

    auto projection = fra::ProjectionUniformBuffer {
        .view = glm::lookAt(cameraPosition, cameraPosition + cameraForward,
                            transform.GetUpDirection()),
        .projection = glm::perspective(
            glm::radians(60.0f),
            (float) mWindow->GetWidth() / (float) mWindow->GetHeight(), 0.1f,
            mRenderer->GetDrawDistance())
    };

    //projection.projection[1][1] *= -1;

    mRenderer->UpdateProjection(projection);
}