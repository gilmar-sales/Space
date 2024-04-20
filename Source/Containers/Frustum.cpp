#include "Frustum.hpp"

Frustum::Frustum(glm::vec3 position, glm::vec3 forward, glm::vec3 right, glm::vec3 up, float aspect, float fovY, float zNear, float zFar)
{
    const float     halfVSide    = zFar * tanf(fovY * .5f);
    const float     halfHSide    = halfVSide * aspect;
    const glm::vec3 frontMultFar = zFar * forward;

    nearFace   = { position + zNear * forward, forward };
    farFace    = { position + frontMultFar, -forward };
    rightFace  = { position,
                   glm::cross(frontMultFar - right * halfHSide, up) };
    leftFace   = { position,
                   glm::cross(up, frontMultFar + right * halfHSide) };
    topFace    = { position,
                   glm::cross(right, frontMultFar - up * halfVSide) };
    bottomFace = { position,
                   glm::cross(frontMultFar + up * halfVSide, right) };
}