#pragma once

#include <Freya/Freya.hpp>

class AssetManager final
{
  public:
    AssetManager(const Ref<fra::MeshPool>&     meshPool,
                 const Ref<fra::TexturePool>&  texturePool,
                 const Ref<fra::MaterialPool>& materialPool);

    ~AssetManager() = default;

    std::uint32_t               GetBlankTexture() { return mBlankTexture; }
    std::uint32_t               GetBlankMaterial() { return mBlankMaterial; }
    std::vector<std::uint32_t>& GetCheckpointModel() { return mCheckpointModel; }
    std::vector<std::uint32_t>& GetPlayerShipModel() { return mPlayerShipModel; }
    std::uint32_t               GetPlayerShipMaterial() { return mPlayerShipMaterial; }
    std::vector<std::uint32_t>& GetEnemyShipModel() { return mEnemyShipModel; }
    std::uint32_t               GetEnemyShipMaterial() { return mEnemyShipMaterial; }
    std::vector<std::uint32_t>& GetMoonModel() { return mMoonModel; }
    std::uint32_t               GetMoonMaterial() { return mMoonMaterial; }
    std::vector<std::uint32_t>& GetJupiterModel() { return mJupiter; }
    std::uint32_t               GetJupiterMaterial() { return mJupiterMaterial; }
    std::vector<std::uint32_t>& GetRock1Model() { return mRock1Model; }
    std::uint32_t               GetRock1Material() { return mRock1Material; }
    std::vector<std::uint32_t>& GetRock2Model() { return mRock2Model; }
    std::uint32_t               GetRock2Material() { return mRock2Material; }
    std::vector<std::uint32_t>& GetBulletModel() { return mBulletModel; }
    std::uint32_t               GetBulletMaterial() { return mBulletMaterial; }

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
    std::uint32_t              mJupiterMaterial;

    std::vector<std::uint32_t> mRock1Model;
    std::uint32_t              mRock1Material;

    std::vector<std::uint32_t> mRock2Model;
    std::uint32_t              mRock2Material;

    std::vector<std::uint32_t> mMoonModel;
    std::uint32_t              mMoonMaterial;

    std::vector<std::uint32_t> mPlayerShipModel;
    std::uint32_t              mPlayerShipMaterial;

    std::vector<uint32_t> mBulletModel;
    uint32_t              mBulletTexture;
    uint32_t              mBulletMaterial;
};