#pragma once

#include <Freya/Freya.hpp>

#include <vector>

class AssetManager final
{
  public:
    AssetManager(const skr::Arc<fra::MeshPool>& meshPool, const skr::Arc<fra::TexturePool>& texturePool,
                 const skr::Arc<fra::MaterialPool>& materialPool);

    ~AssetManager() = default;

    fra::TextureHandle             GetBlankTexture() { return mBlankTexture; }
    fra::MaterialHandle            GetBlankMaterial() { return mBlankMaterial; }
    std::vector<fra::MeshHandle>&  GetCheckpointModel() { return mCheckpointModel; }
    std::vector<fra::MeshHandle>&  GetPlayerShipModel() { return mPlayerShipModel; }
    fra::MaterialHandle            GetPlayerShipMaterial() { return mPlayerShipMaterial; }
    std::vector<fra::MeshHandle>&  GetEnemyShipModel() { return mEnemyShipModel; }
    fra::MaterialHandle            GetEnemyShipMaterial() { return mEnemyShipMaterial; }
    std::vector<fra::MeshHandle>&  GetMoonModel() { return mMoonModel; }
    fra::MaterialHandle            GetMoonMaterial() { return mMoonMaterial; }
    std::vector<fra::MeshHandle>&  GetJupiterModel() { return mJupiter; }
    fra::MaterialHandle            GetJupiterMaterial() { return mJupiterMaterial; }
    std::vector<fra::MeshHandle>&  GetRock1Model() { return mRock1Model; }
    fra::MaterialHandle            GetRock1Material() { return mRock1Material; }
    std::vector<fra::MeshHandle>&  GetRock2Model() { return mRock2Model; }
    fra::MaterialHandle            GetRock2Material() { return mRock2Material; }
    std::vector<fra::MeshHandle>&  GetBulletModel() { return mBulletModel; }
    fra::MaterialHandle            GetBulletMaterial() { return mBulletMaterial; }
    fra::MaterialHandle            GetEnemyBulletMaterial() { return mEnemyBulletMaterial; }

  private:
    skr::Arc<fra::MeshPool>     mMeshPool;
    skr::Arc<fra::TexturePool>  mTexturePool;
    skr::Arc<fra::MaterialPool> mMaterialPool;

    fra::TextureHandle  mBlankTexture;
    fra::MaterialHandle mBlankMaterial;

    std::vector<fra::MeshHandle> mCheckpointModel;

    std::vector<fra::MeshHandle> mEnemyShipModel;
    fra::MaterialHandle          mEnemyShipMaterial;

    std::vector<fra::MeshHandle> mJupiter;
    fra::MaterialHandle          mJupiterMaterial;

    std::vector<fra::MeshHandle> mRock1Model;
    fra::MaterialHandle          mRock1Material;

    std::vector<fra::MeshHandle> mRock2Model;
    fra::MaterialHandle          mRock2Material;

    std::vector<fra::MeshHandle> mMoonModel;
    fra::MaterialHandle          mMoonMaterial;

    std::vector<fra::MeshHandle> mPlayerShipModel;
    fra::MaterialHandle          mPlayerShipMaterial;

    std::vector<fra::MeshHandle> mBulletModel;
    fra::MaterialHandle          mBulletMaterial;
    fra::MaterialHandle          mEnemyBulletMaterial;
};
