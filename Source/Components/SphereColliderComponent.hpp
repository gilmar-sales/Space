#pragma once

#include <Freyr.hpp>
#include <glm/glm.hpp>

struct SphereColliderComponent : fr::Component
{
    float     radius;
    glm::vec3 offset;
};