#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

class SpawnSystem final : public fr::System
{
  public:
    explicit SpawnSystem(
        const std::shared_ptr<fr::Scene>&         scene,
        const std::shared_ptr<fra::MeshPool>&     meshPool,
        const std::shared_ptr<fra::TexturePool>&  texturePool,
        const std::shared_ptr<fra::MaterialPool>& materialPool);

  private:
    std::shared_ptr<fra::MeshPool>     mMeshPool;
    std::shared_ptr<fra::TexturePool>  mTexturePool;
    std::shared_ptr<fra::MaterialPool> mMaterialPool;

    std::uint32_t mBlankTexture;
    std::uint32_t mBlankMaterial;

    std::vector<std::uint32_t> mCheckpointModel;

    std::vector<std::uint32_t> mEnemyShipModel;
    std::uint32_t              mEnemyShipMaterial;

    std::vector<std::uint32_t> mJupiter;
    std::uint32_t              mSunMaterial;

    std::vector<std::uint32_t> mRock1Model;
    std::uint32_t              mRock1Material;

    std::vector<std::uint32_t> mRock2Model;
    std::uint32_t              mRock2Material;

    std::vector<std::uint32_t> mMoonModel;
    std::uint32_t              mMoonMaterial;

    std::vector<std::uint32_t> mPlayerShipModel;
    std::uint32_t              mPlayerShipMaterial;
};