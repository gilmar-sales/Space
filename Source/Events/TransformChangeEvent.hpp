#pragma once

#include <Freyr/Freyr.hpp>

struct TransformChangeEvent : fr::Event
{
    fr::Entity entity;
};