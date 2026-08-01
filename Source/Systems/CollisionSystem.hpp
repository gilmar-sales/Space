#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

#include <Systems/OctreeSystem.hpp>

class CollisionSystem final : public fr::System
{
  public:
    CollisionSystem(const skr::Arc<fr::Registry>& registry,
                    const skr::Arc<fra::Renderer>& renderer,
                    const skr::Arc<fra::MeshPool>& meshPool,
                    const skr::Arc<OctreeSystem>&  octreeSystem) :
        System(registry), mRenderer(renderer), mMeshPool(meshPool), mOctreeSystem(octreeSystem)
    {
    }

    ~CollisionSystem() override = default;

    void Update(float deltaTime) override;

  private:
    friend class SpaceApp;

    std::vector<std::uint32_t> mSphereModel;
    std::vector<std::uint32_t> mCubeModel;

    skr::Arc<fra::Renderer> mRenderer;
    skr::Arc<fra::MeshPool> mMeshPool;
    skr::Arc<OctreeSystem>  mOctreeSystem;
};