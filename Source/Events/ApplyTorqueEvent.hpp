#pragma once

#include <Freyr/Freyr.hpp>

struct ApplyTorqueEvent : fr::Event
{
    fr::Entity target;
    glm::vec3  axis;
    float      magnetiude;
    float      deltaTime;
};