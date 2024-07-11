#pragma once

#include <Freyr/Freyr.hpp>

#include <Asset/MeshPool.hpp>

class SpawnSystem final : public fr::System
{
  public:
    explicit SpawnSystem(const std::shared_ptr<fra::MeshPool>& meshPool) : mMeshPool(meshPool)
    {
    }

    void Start() override;

  private:
    std::shared_ptr<fra::MeshPool> mMeshPool;

    std::vector<std::uint32_t> mBlackHoleModel;
    std::vector<std::uint32_t> mMoonModel;
    std::vector<std::uint32_t> mCheckpointModel;
    std::vector<std::uint32_t> mRedShipModel;
    std::vector<std::uint32_t> mBlueShipModel;
    std::vector<std::uint32_t> mXWingModel;
};