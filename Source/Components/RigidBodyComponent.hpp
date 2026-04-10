#pragma once

#include "Containers/Octree.hpp"

#include <Freyr/Freyr.hpp>

#include <glm/glm.hpp>

struct RigidBodyComponent : fr::Component
{
    bool      isKinematic     = false;
    bool      kinematicIfStop = false;
    float     mass            = 1.0f;
    glm::vec3 velocity        = glm::vec3(0.0f);

    void ApplyForce(const glm::vec3& direction, float magnitude, float dt)
    {
        if (isKinematic)
            return;

        glm::vec3 force = direction * magnitude;

        glm::vec3 acceleration = force / mass;

        velocity += acceleration * dt;
    }
};
