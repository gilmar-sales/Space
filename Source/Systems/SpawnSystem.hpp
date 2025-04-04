#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

class SpawnSystem final : public fr::System
{
  public:
    explicit SpawnSystem(const Ref<fr::Scene>&         scene,
                         const Ref<fra::MeshPool>&     meshPool,
                         const Ref<fra::TexturePool>&  texturePool,
                         const Ref<fra::MaterialPool>& materialPool);

  private:
    Ref<fra::MeshPool>     mMeshPool;
    Ref<fra::TexturePool>  mTexturePool;
    Ref<fra::MaterialPool> mMaterialPool;

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