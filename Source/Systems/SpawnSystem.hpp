#pragma once

#include "Asset/TexturePool.hpp"

#include <Freyr/Freyr.hpp>

#include <Asset/MeshPool.hpp>

class SpawnSystem final : public fr::System
{
  public:
    explicit SpawnSystem(const std::shared_ptr<fr::Scene>&        scene,
                         const std::shared_ptr<fra::MeshPool>&    meshPool,
                         const std::shared_ptr<fra::TexturePool>& texturePool);

  private:
    std::shared_ptr<fra::MeshPool>    mMeshPool;
    std::shared_ptr<fra::TexturePool> mTexturePool;

    std::uint32_t mBlankTexture;

    std::vector<std::uint32_t> mCheckpointModel;

    std::vector<std::uint32_t> mEnemyShipModel;
    std::uint32_t              mEnemyShipTexture;

    std::vector<std::uint32_t> mBlackHoleModel;

    std::vector<std::uint32_t> mSunModel;
    std::uint32_t              mSunTexture;

    std::vector<std::uint32_t> mMoonModel;
    std::uint32_t              mMoonTexture;

    std::vector<std::uint32_t> mPlayerShipModel;
    std::uint32_t              mPlayerShipTexture;
};