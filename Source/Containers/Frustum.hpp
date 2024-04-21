#pragma once

#include <glm/glm.hpp>

struct Plane
{
    glm::vec3 normal;
    float     distance;

    Plane() = default;

    Plane(const glm::vec3& position, const glm::vec3& normal) :
        normal(glm::normalize(normal)), distance(glm::dot(normal, position))
    {
    }

    float getSignedDistanceToPlane(const glm::vec3& point) const { return glm::dot(normal, point) - distance; }
};

struct Frustum
{
    Plane topFace;
    Plane bottomFace;
    Plane rightFace;
    Plane leftFace;
    Plane farFace;
    Plane nearFace;

    Frustum(glm::vec3 position, glm::vec3 forward, glm::vec3 right, glm::vec3 up, float aspect, float fovY, float zNear,
            float zFar);
};