#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

class PlayerControlSystem final : public fr::System
{
  public:
    PlayerControlSystem(const skr::Arc<fr::Registry>& registry, const skr::Arc<fra::EventManager>& eventManger);
};