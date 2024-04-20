#pragma once

#include <Core/Renderer.hpp>
#include <Freyr/Freyr.hpp>

#include "Containers/Octree.hpp"
#include <Systems/OctreeSystem.hpp>

class CollisionSystem : public fr::System
{
  public:
    CollisionSystem(std::shared_ptr<fra::Renderer> renderer, std::shared_ptr<OctreeSystem> octreeSystem) :
        mRenderer(renderer), mOctreeSystem(octreeSystem) {}

    void Start() override;
    void Update(float deltaTime) override;

  private:
    friend class SpaceApp;

    std::vector<std::uint32_t>     mSphereModel;
    std::vector<std::uint32_t>     mCubeModel;
    std::shared_ptr<OctreeSystem>  mOctreeSystem;
    std::shared_ptr<fra::Renderer> mRenderer;
};