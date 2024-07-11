#pragma once

#include <Freyr/Freyr.hpp>

#include <glm/glm.hpp>

struct RigidBodyComponent : fr::Component
{
    float     mass;
    glm::vec3 velocity;
};