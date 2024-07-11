#pragma once

#include <Core/Renderer.hpp>
#include <Freyr/Freyr.hpp>

#include <Systems/OctreeSystem.hpp>

class CollisionSystem final : public fr::System
{
  public:
    CollisionSystem(const std::shared_ptr<fra::Renderer>& renderer,
                    const std::shared_ptr<fra::MeshPool>& meshPool,
                    const std::shared_ptr<OctreeSystem>&  octreeSystem) :
        mRenderer(renderer),
        mMeshPool(meshPool), mOctreeSystem(octreeSystem)
    {
    }

    virtual ~CollisionSystem() = default;

    void Start() override;
    void Update(float deltaTime) override;

  private:
    friend class SpaceApp;

    std::vector<std::uint32_t> mSphereModel;
    std::vector<std::uint32_t> mCubeModel;

    std::shared_ptr<fra::Renderer> mRenderer;
    std::shared_ptr<fra::MeshPool> mMeshPool;
    std::shared_ptr<OctreeSystem>  mOctreeSystem;
};