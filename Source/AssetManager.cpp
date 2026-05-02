#include "AssetManager.hpp"

AssetManager::AssetManager(const Ref<fra::MeshPool>& meshPool, const Ref<fra::TexturePool>& texturePool,
                           const Ref<fra::MaterialPool>& materialPool) :
    mMeshPool(meshPool), mTexturePool(texturePool), mMaterialPool(materialPool)
{
    mBlankTexture  = mTexturePool->CreateTextureFromFile("./Resources/Textures/blank_texture.png");
    mBlankMaterial = mMaterialPool->Create({ mBlankTexture, mBlankTexture, mBlankTexture });

    mPlayerShipModel    = mMeshPool->CreateMeshFromFile("./Resources/Models/ally_ship.glb");
    mPlayerShipMaterial = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/player_ship.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/player_ship_normal.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/player_ship_roughness.png") });

    mEnemyShipModel    = mMeshPool->CreateMeshFromFile("./Resources/Models/enemy_ship.glb");
    mEnemyShipMaterial = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/enemy_ship.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/enemy_ship_normal.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/enemy_ship_roughness.png") });

    mMoonModel    = mMeshPool->CreateMeshFromFile("./Resources/Models/quad_sphere.glb");
    mMoonMaterial = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/moon.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/moon_normal.png"), mBlankTexture });

    mJupiter         = mMeshPool->CreateMeshFromFile("./Resources/Models/quad_sphere.glb");
    mJupiterMaterial = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/jupiter.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/jupiter_normal.png"), mBlankTexture });

    mRock1Model    = mMeshPool->CreateMeshFromFile("./Resources/Models/rock_01.glb");
    mRock1Material = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_01.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_01_normal.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_01_roughness.png") });

    mRock2Model    = mMeshPool->CreateMeshFromFile("./Resources/Models/rock_02.glb");
    mRock2Material = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_02.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_02_normal.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_02_roughness.png") });

    mCheckpointModel = mMeshPool->CreateMeshFromFile("./Resources/Models/checkpoint.glb");

    mBulletModel    = meshPool->CreateMeshFromFile("./Resources/Models/quad_sphere.glb");
    mBulletTexture  = texturePool->CreateTextureFromFile("./Resources/Textures/blank_texture.png");
    mBulletMaterial = materialPool->Create({ mBulletTexture, mBulletTexture, mBulletTexture });
}