#pragma once

#include <Freyr/Freyr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

struct TransformComponent : fr::Component
{
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    glm::vec3 GetForwardDirection() const
    {
        return glm::vec3(glm::vec4(0, 0, 1, 0) * rotation);
    }

    glm::vec3 GetRightDirection() const
    {
        return glm::normalize(
            glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), GetForwardDirection()));
    }

    glm::vec3 GetUpDirection() const
    {
        return glm::normalize(
            glm::cross(GetForwardDirection(), GetRightDirection()));
    }

    glm::mat4 GetModel() const
    {
        auto translateM = glm::translate(glm::mat4(1), position);
        auto rotateM    = glm::mat4_cast(rotation);
        auto scaleM     = glm::scale(glm::mat4(1), scale);

        return translateM * rotateM * scaleM;
    }
};