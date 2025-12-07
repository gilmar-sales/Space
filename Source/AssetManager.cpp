#include "AssetManager.hpp"

AssetManager::AssetManager(const Ref<fra::MeshPool>& meshPool, const Ref<fra::TexturePool>& texturePool,
                           const Ref<fra::MaterialPool>& materialPool) :
    mMeshPool(meshPool), mTexturePool(texturePool), mMaterialPool(materialPool)
{
    mBlankTexture  = mTexturePool->CreateTextureFromFile("./Resources/Textures/blank_texture.png");
    mBlankMaterial = mMaterialPool->Create({ mBlankTexture, mBlankTexture, mBlankTexture });

    mPlayerShipModel    = mMeshPool->CreateMeshFromFile("./Resources/Models/player_ship.fbx");
    mPlayerShipMaterial = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/player_ship.jpg"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/player_ship_normal.jpg"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/player_ship_roughness.jpg") });

    mEnemyShipModel    = mMeshPool->CreateMeshFromFile("./Resources/Models/enemy_ship.fbx");
    mEnemyShipMaterial = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/enemy_ship.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/enemy_ship_normal.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/enemy_ship_roughness.png") });

    mMoonModel    = mMeshPool->CreateMeshFromFile("./Resources/Models/moon.fbx");
    mMoonMaterial = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/moon.jpg"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/moon_normal.png"), mBlankTexture });

    mJupiter         = mMeshPool->CreateMeshFromFile("./Resources/Models/sun.fbx");
    mJupiterMaterial = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/jupiter.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/jupiter_normal.png"), mBlankTexture });

    mRock1Model    = mMeshPool->CreateMeshFromFile("./Resources/Models/rock_01.fbx");
    mRock1Material = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_01.tga"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_01_normal.tga"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_01_roughness.tga") });

    mRock2Model    = mMeshPool->CreateMeshFromFile("./Resources/Models/rock_02.fbx");
    mRock2Material = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_02.tga"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_02_normal.tga"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_02_roughness.tga") });

    mCheckpointModel = mMeshPool->CreateMeshFromFile("./Resources/Models/checkpoint.fbx");

    mBulletModel    = meshPool->CreateMeshFromFile("./Resources/Models/moon.fbx");
    mBulletTexture  = texturePool->CreateTextureFromFile("./Resources/Textures/blank_texture.png");
    mBulletMaterial = materialPool->Create({ mBulletTexture, mBulletTexture, mBulletTexture });
}