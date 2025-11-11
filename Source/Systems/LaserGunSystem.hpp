
#pragma once

#include "Events/CollisionEvent.hpp"
#include "OctreeSystem.hpp"

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

class LaserGunSystem : public fr::System
{
  public:
    LaserGunSystem(const Ref<fr::Scene>&         scene,
                   const Ref<fra::MeshPool>&     meshPool,
                   const Ref<fra::TexturePool>&  texturePool,
                   const Ref<fra::MaterialPool>& materialPool,
                   const Ref<OctreeSystem>&      octreeSystem) : System(scene), mOctreeSystem(octreeSystem)
    {
        mBulletModel    = meshPool->CreateMeshFromFile("./Resources/Models/moon.fbx");
        mBulletTexture  = texturePool->CreateTextureFromFile("./Resources/Textures/blank_texture.png");
        mBulletMaterial = materialPool->Create({ mBulletTexture, mBulletTexture, mBulletTexture });

        mScene->AddEventListener<CollisionEvent>([this](const CollisionEvent event) { OnCollision(event); });
    }

    void Update(float deltaTime) override;

    void OnCollision(const CollisionEvent& event) const;

  private:
    std::vector<uint32_t> mBulletModel;
    uint32_t              mBulletTexture;
    uint32_t              mBulletMaterial;
    Ref<OctreeSystem>     mOctreeSystem;
};
