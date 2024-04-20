#pragma once

#include <Freyr/Freyr.hpp>
#include <glm/glm.hpp>

struct TransformComponent : fr::Component
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};