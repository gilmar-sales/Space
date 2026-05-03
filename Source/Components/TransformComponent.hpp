#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

static constexpr glm::vec3 WORLD_FORWARD { 0.0f, 0.0f, 1.0f };
static constexpr glm::vec3 WORLD_RIGHT { 1.0f, 0.0f, 0.0f };
static constexpr glm::vec3 WORLD_UP { 0.0f, 1.0f, 0.0f };

struct TransformComponent : fr::Component
{
    glm::vec3 position { glm::vec3(0.0f) };
    glm::quat rotation { glm::identity<glm::quat>() };
    glm::vec3 scale { glm::vec3(1.0f) };

    [[nodiscard]] glm::vec3 GetForwardDirection() const { return WORLD_FORWARD * rotation; }
    [[nodiscard]] glm::vec3 GetRightDirection() const { return WORLD_RIGHT * rotation; }
    [[nodiscard]] glm::vec3 GetUpDirection() const { return WORLD_UP * rotation; }

    [[nodiscard]] glm::mat4 GetModel() const
    {
        return glm::translate(glm::identity<glm::mat4>(), position) * glm::mat4(glm::inverse(rotation)) *
               glm::scale(glm::identity<glm::mat4>(), scale);
    }

    void Rotate(const glm::vec3& axis, const float angleDegPerSec, const float deltaTime)
    {
        const float deltaAngle = glm::radians(angleDegPerSec) * deltaTime;

        rotation = glm::normalize(rotation * glm::angleAxis(deltaAngle, axis));
    }

    void Reset()
    {
        position = glm::vec3(0.0f);
        rotation = glm::identity<glm::quat>();
        scale    = glm::vec3(1.0f);
    }
};
