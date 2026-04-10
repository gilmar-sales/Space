#include "Frustum.hpp"

Frustum::Frustum(const glm::mat4& viewProjMatrix)
{

    // Left plane
    planes[0].normal.x = viewProjMatrix[0][3] + viewProjMatrix[0][0];
    planes[0].normal.y = viewProjMatrix[1][3] + viewProjMatrix[1][0];
    planes[0].normal.z = viewProjMatrix[2][3] + viewProjMatrix[2][0];
    planes[0].distance = viewProjMatrix[3][3] + viewProjMatrix[3][0];

    // Right plane
    planes[1].normal.x = viewProjMatrix[0][3] - viewProjMatrix[0][0];
    planes[1].normal.y = viewProjMatrix[1][3] - viewProjMatrix[1][0];
    planes[1].normal.z = viewProjMatrix[2][3] - viewProjMatrix[2][0];
    planes[1].distance = viewProjMatrix[3][3] - viewProjMatrix[3][0];

    // Bottom plane
    planes[2].normal.x = viewProjMatrix[0][3] + viewProjMatrix[0][1];
    planes[2].normal.y = viewProjMatrix[1][3] + viewProjMatrix[1][1];
    planes[2].normal.z = viewProjMatrix[2][3] + viewProjMatrix[2][1];
    planes[2].distance = viewProjMatrix[3][3] + viewProjMatrix[3][1];

    // Top plane
    planes[3].normal.x = viewProjMatrix[0][3] - viewProjMatrix[0][1];
    planes[3].normal.y = viewProjMatrix[1][3] - viewProjMatrix[1][1];
    planes[3].normal.z = viewProjMatrix[2][3] - viewProjMatrix[2][1];
    planes[3].distance = viewProjMatrix[3][3] - viewProjMatrix[3][1];

    // Near plane
    planes[4].normal.x = viewProjMatrix[0][3] + viewProjMatrix[0][2];
    planes[4].normal.y = viewProjMatrix[1][3] + viewProjMatrix[1][2];
    planes[4].normal.z = viewProjMatrix[2][3] + viewProjMatrix[2][2];
    planes[4].distance = viewProjMatrix[3][3] + viewProjMatrix[3][2];

    // Far plane
    planes[5].normal.x = viewProjMatrix[0][3] - viewProjMatrix[0][2];
    planes[5].normal.y = viewProjMatrix[1][3] - viewProjMatrix[1][2];
    planes[5].normal.z = viewProjMatrix[2][3] - viewProjMatrix[2][2];
    planes[5].distance = viewProjMatrix[3][3] - viewProjMatrix[3][2];

    // Normalize the planes
    for (int i = 0; i < 6; i++)
    {
        float length = glm::length(planes[i].normal);
        planes[i].normal /= length;
        planes[i].distance /= length;
    }
}

bool Frustum::SphereIntersect(glm::vec3& position, float radius) const
{
    for (int i = 0; i < 6; i++)
    {
        float distance =
            glm::dot(planes[i].normal, position) + planes[i].distance;

        if (distance < -radius)
        {
            return false;
        }
    }
    return true;
}

bool Frustum::PointInFrustum(const glm::vec3& point) const
{
    for (int i = 0; i < 6; i++)
    {
        if (glm::dot(planes[i].normal, point) + planes[i].distance < 0)
        {
            return false;
        }
    }

    return true;
}
