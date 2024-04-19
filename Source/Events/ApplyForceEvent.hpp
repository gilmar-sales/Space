#pragma once

#include <Freyr.hpp>

#include <glm/glm.hpp>

struct ApplyForceEvent : fr::Event
{
    fr::Entity target;
    glm::vec3 direction;
    float magnetiude;
    float deltaTime;
};
