#pragma once

#include <glm/glm.hpp>

struct Plane
{
    glm::vec3 normal;   // Normal vector to the plane
    float     distance; // Distance from the origin
};

struct Frustum
{
    Plane planes[6];

    Frustum(const glm::mat4& viewProjMatrix);

    bool SphereIntersect(glm::vec3& position, float radius) const;
};