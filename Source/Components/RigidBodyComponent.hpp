#pragma once

#include <Freyr.hpp>

struct RigidBodyComponent : fr::Component
{
    float mass;
    glm::vec3 velocity;
};