#include "AssetManager.hpp"

#include <vector>

namespace
{
std::vector<std::uint32_t> MeshIdsFromModel(std::vector<fra::ModelSubmesh> parts)
{
    std::vector<std::uint32_t> ids;
    ids.reserve(parts.size());
    for (const auto& part : parts)
        ids.push_back(part.meshId);
    return ids;
}
} // namespace

AssetManager::AssetManager(const skr::Arc<fra::MeshPool>& meshPool, const skr::Arc<fra::TexturePool>& texturePool,
                           const skr::Arc<fra::MaterialPool>& materialPool) :
    mMeshPool(meshPool), mTexturePool(texturePool), mMaterialPool(materialPool)
{
    mBlankTexture = mTexturePool->CreateTextureFromFile("./Resources/Textures/blank_texture.png").value_or(0);
    mBlankMaterial = mMaterialPool->Create({ mBlankTexture, mBlankTexture, mBlankTexture });

    mPlayerShipModel = MeshIdsFromModel(mMeshPool->CreateModelFromFile("./Resources/Models/ally_ship.glb"));
    mPlayerShipMaterial = mMaterialPool->Create(
        { .albedo = mTexturePool->CreateTextureFromFile("./Resources/Textures/player_ship.png"),
          .normal = mTexturePool->CreateTextureFromFile("./Resources/Textures/player_ship_normal.png"),
          .roughness =
              mTexturePool->CreateTextureFromFile("./Resources/Textures/player_ship_roughness.png"),
          .roughnessFactor = 4.0f,
          .metalnessFactor = 0.0f });

    mEnemyShipModel = MeshIdsFromModel(mMeshPool->CreateModelFromFile("./Resources/Models/enemy_ship.glb"));
    mEnemyShipMaterial = mMaterialPool->Create(
        { .albedo = mTexturePool->CreateTextureFromFile("./Resources/Textures/enemy_ship.png"),
          .normal = mTexturePool->CreateTextureFromFile("./Resources/Textures/enemy_ship_normal.png"),
          .roughness =
              mTexturePool->CreateTextureFromFile("./Resources/Textures/enemy_ship_roughness.png"),
          .roughnessFactor = 2.0f,
          .metalnessFactor = 0.0f });

    mMoonModel = MeshIdsFromModel(mMeshPool->CreateModelFromFile("./Resources/Models/quad_sphere.glb"));
    mMoonMaterial = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/moon.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/moon_normal.png"), mBlankTexture });

    mJupiter = MeshIdsFromModel(mMeshPool->CreateModelFromFile("./Resources/Models/quad_sphere.glb"));
    mJupiterMaterial = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/jupiter.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/jupiter_normal.png"), mBlankTexture });

    mRock1Model = MeshIdsFromModel(mMeshPool->CreateModelFromFile("./Resources/Models/rock_01.glb"));
    mRock1Material = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_01.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_01_normal.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_01_roughness.png") });

    mRock2Model = MeshIdsFromModel(mMeshPool->CreateModelFromFile("./Resources/Models/rock_02.glb"));
    mRock2Material = mMaterialPool->Create(
        { mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_02.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_02_normal.png"),
          mTexturePool->CreateTextureFromFile("./Resources/Textures/rock_02_roughness.png") });

    mCheckpointModel = MeshIdsFromModel(mMeshPool->CreateModelFromFile("./Resources/Models/checkpoint.glb"));

    mBulletModel = MeshIdsFromModel(meshPool->CreateModelFromFile("./Resources/Models/laser_shot.glb"));
    auto blueTexture = texturePool->CreateTextureFromFile("./Resources/Textures/blue_texture.png");
    auto redTexture  = texturePool->CreateTextureFromFile("./Resources/Textures/red_texture.png");
    mBulletMaterial = materialPool->Create({ .albedo = mBlankTexture, .emissive = blueTexture });
    mEnemyBulletMaterial = materialPool->Create({ .albedo = mBlankTexture, .emissive = redTexture });
}
