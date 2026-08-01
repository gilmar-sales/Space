#pragma once

#include "Events/CollisionEvent.hpp"
#include "OctreeSystem.hpp"

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

#include <Components/PlayerComponent.hpp>

#include <AssetManager.hpp>
#include <Random.hpp>

class LaserGunSystem : public fr::System {
public:
    LaserGunSystem(const skr::Arc<fr::Registry> &registry,
                   const skr::Arc<OctreeSystem> &octreeSystem,
                   const skr::Arc<AssetManager> &assetManager,
                   const skr::Arc<Random> &random) : System(registry), mOctreeSystem(octreeSystem), mAssetManager(assetManager),
                                                mRandom(random) {
        mPlayer = mRegistry->CreateQuery()->FindUnique<PlayerComponent>();
        mCollisionListener =
                mRegistry->AddEventListener<CollisionEvent>([this](const CollisionEvent event) { OnCollision(event); });
    }

    void Update(float deltaTime) override;

    void OnCollision(const CollisionEvent &event) const;

    void Shoot(fr::Entity owner, std::uint32_t material, glm::vec3 position, glm::quat rotation, glm::vec3 velocity);

private:
    std::optional<fr::Entity> mPlayer;
    skr::Arc<fr::ListenerHandle> mCollisionListener;
    skr::Arc<OctreeSystem> mOctreeSystem;
    skr::Arc<AssetManager> mAssetManager;
    skr::Arc<Random> mRandom;
};
