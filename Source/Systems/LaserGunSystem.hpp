
#pragma once

#include "Events/CollisionEvent.hpp"
#include "OctreeSystem.hpp"

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

#include <AssetManager.hpp>
#include <Random.hpp>

class LaserGunSystem : public fr::System
{
  public:
    LaserGunSystem(const Ref<fr::Scene>&    scene,
                   const Ref<OctreeSystem>& octreeSystem,
                   const Ref<AssetManager>& assetManager,
                   const Ref<Random>&       random) :
        System(scene), mOctreeSystem(octreeSystem), mAssetManager(assetManager), mRandom(random)
    {
        mScene->AddEventListener<CollisionEvent>([this](const CollisionEvent event) { OnCollision(event); });
    }

    void Update(float deltaTime) override;

    void OnCollision(const CollisionEvent& event) const;

  private:
    Ref<OctreeSystem> mOctreeSystem;
    Ref<AssetManager> mAssetManager;
    Ref<Random>       mRandom;
};
