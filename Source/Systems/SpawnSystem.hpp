#pragma once

#include <Freyr.hpp>

class SpawnSystem : public fr::System
{
  public:
    void Start() override;
    void Update(float dt) override;

  private:
    std::vector<std::uint32_t> mMoonModel;
    std::vector<std::uint32_t> mCheckpointModel;
    std::vector<std::uint32_t> mRedShipModel;
    std::vector<std::uint32_t> mBlueShipModel;
};