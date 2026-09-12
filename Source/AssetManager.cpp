#include "AssetManager.hpp"

#include <print>
#include <vector>

namespace
{
constexpr fra::MeshLodBuildOptions kShipMeshLods {
    .enabled     = true,
    .ratios      = { 0.18f, 0.06f, 0.02f },
    .targetError = 0.05f,
};

constexpr fra::MeshLodBuildOptions kPropMeshLods {
    .enabled     = true,
    .ratios      = { 0.25f, 0.10f, 0.04f },
    .targetError = 0.04f,
};

std::vector<fra::MeshHandle> MeshHandlesFromModel(
    const skr::Arc<fra::MeshPool>&        meshPool,
    const std::vector<fra::ModelSubmesh>& parts, const char* label)
{
    std::vector<fra::MeshHandle> ids;
    ids.reserve(parts.size());
    for (std::size_t i = 0; i < parts.size(); ++i)
    {
        const auto& part = parts[i];
        ids.push_back(part.mesh);
        if (meshPool && part.mesh.IsValid())
        {
            const auto& mesh = meshPool->GetMesh(part.mesh);
            std::println("[MeshLod] {}[{}] lods={} lod0Indices={}", label, i,
                         mesh.lodCount, mesh.indexCount);
        }
    }
    return ids;
}
} // namespace

AssetManager::AssetManager(const skr::Arc<fra::MeshPool>& meshPool,
                           const skr::Arc<fra::TexturePool>& texturePool,
                           const skr::Arc<fra::MaterialPool>& materialPool) :
    mMeshPool(meshPool), mTexturePool(texturePool), mMaterialPool(materialPool)
{
    mBlankTexture =
        mTexturePool->CreateTextureFromFile("./Resources/Textures/blank_texture.png")
            .value_or(fra::TextureHandle {});
    mBlankMaterial = mMaterialPool->Create({ .albedo    = mBlankTexture,
                                             .normal    = mBlankTexture,
                                             .roughness = mBlankTexture });

    mPlayerShipModel = MeshHandlesFromModel(
        mMeshPool,
        mMeshPool->CreateModelFromFile("./Resources/Models/ally_ship.glb",
                                       kShipMeshLods),
        "ally_ship");
    mPlayerShipMaterial = mMaterialPool->Create(
        { .albedo = mTexturePool->CreateTextureFromFile(
              "./Resources/Textures/player_ship.png"),
          .normal = mTexturePool->CreateTextureFromFile(
              "./Resources/Textures/player_ship_normal.png"),
          .roughness = mTexturePool->CreateTextureFromFile(
              "./Resources/Textures/player_ship_roughness.png"),
          .roughnessFactor = 4.0f,
          .metalnessFactor = 0.0f });

    mEnemyShipModel = MeshHandlesFromModel(
        mMeshPool,
        mMeshPool->CreateModelFromFile("./Resources/Models/enemy_ship.glb",
                                       kShipMeshLods),
        "enemy_ship");
    mEnemyShipMaterial = mMaterialPool->Create(
        { .albedo = mTexturePool->CreateTextureFromFile(
              "./Resources/Textures/enemy_ship.png"),
          .normal = mTexturePool->CreateTextureFromFile(
              "./Resources/Textures/enemy_ship_normal.png"),
          .roughness = mTexturePool->CreateTextureFromFile(
              "./Resources/Textures/enemy_ship_roughness.png"),
          .roughnessFactor = 2.0f,
          .metalnessFactor = 0.0f });

    mMoonModel = MeshHandlesFromModel(
        mMeshPool,
        mMeshPool->CreateModelFromFile("./Resources/Models/quad_sphere.glb",
                                       kPropMeshLods),
        "moon");
    mMoonMaterial = mMaterialPool->Create(
        { .albedo = mTexturePool->CreateTextureFromFile(
              "./Resources/Textures/moon.png"),
          .normal = mTexturePool->CreateTextureFromFile(
              "./Resources/Textures/moon_normal.png"),
          .roughness = mBlankTexture });

    mJupiter = MeshHandlesFromModel(
        mMeshPool,
        mMeshPool->CreateModelFromFile("./Resources/Models/quad_sphere.glb",
                                       kPropMeshLods),
        "jupiter");
    mJupiterMaterial = mMaterialPool->Create(
        { .albedo = mTexturePool->CreateTextureFromFile(
              "./Resources/Textures/jupiter.png"),
          .normal = mTexturePool->CreateTextureFromFile(
              "./Resources/Textures/jupiter_normal.png"),
          .roughness = mBlankTexture });

    mRock1Model = MeshHandlesFromModel(
        mMeshPool,
        mMeshPool->CreateModelFromFile("./Resources/Models/rock_01.glb",
                                       kPropMeshLods),
        "rock_01");
    mRock1Material = mMaterialPool->Create(
        { .albedo = mTexturePool->CreateTextureFromFile(
              "./Resources/Textures/rock_01.png"),
          .normal = mTexturePool->CreateTextureFromFile(
              "./Resources/Textures/rock_01_normal.png"),
          .roughness = mTexturePool->CreateTextureFromFile(
              "./Resources/Textures/rock_01_roughness.png") });

    mRock2Model = MeshHandlesFromModel(
        mMeshPool,
        mMeshPool->CreateModelFromFile("./Resources/Models/rock_02.glb",
                                       kPropMeshLods),
        "rock_02");
    mRock2Material = mMaterialPool->Create(
        { .albedo = mTexturePool->CreateTextureFromFile(
              "./Resources/Textures/rock_02.png"),
          .normal = mTexturePool->CreateTextureFromFile(
              "./Resources/Textures/rock_02_normal.png"),
          .roughness = mTexturePool->CreateTextureFromFile(
              "./Resources/Textures/rock_02_roughness.png") });

    mCheckpointModel = MeshHandlesFromModel(
        mMeshPool,
        mMeshPool->CreateModelFromFile("./Resources/Models/checkpoint.glb",
                                       kPropMeshLods),
        "checkpoint");

    mBulletModel = MeshHandlesFromModel(
        meshPool,
        meshPool->CreateModelFromFile("./Resources/Models/laser_shot.glb",
                                      { .enabled = false }),
        "bullet");
    auto blueTexture =
        texturePool->CreateTextureFromFile("./Resources/Textures/blue_texture.png");
    auto redTexture =
        texturePool->CreateTextureFromFile("./Resources/Textures/red_texture.png");
    mBulletMaterial =
        materialPool->Create({ .albedo = mBlankTexture, .emissive = blueTexture });
    mEnemyBulletMaterial =
        materialPool->Create({ .albedo = mBlankTexture, .emissive = redTexture });
}
