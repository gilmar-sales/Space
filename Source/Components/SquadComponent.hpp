#pragma once

#include <Freyr/Freyr.hpp>

enum class Squad
{
    Ally,
    Enemy
};

struct SquadComponent : fr::Component
{
    Squad squad;
};