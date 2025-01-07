#pragma once

#include "Containers/Octree.hpp"

#include <Freyr/Freyr.hpp>

#include <glm/glm.hpp>

struct RigidBodyComponent : fr::Component
{
    float           mass;
    glm::vec3       velocity;
    bool            isKinematic;
    mutable Octree* octree;
};