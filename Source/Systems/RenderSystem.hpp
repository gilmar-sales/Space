#pragma once

#include <Core/Renderer.hpp>
#include <Freyr/Freyr.hpp>

#include <Systems/OctreeSystem.hpp>

struct InstanceDraw
{
    size_t                      index;
    int                         instanceCount;
    std::vector<std::uint32_t>* meshes;
};

class RenderSystem : public fr::System
{
  public:
    RenderSystem(std::shared_ptr<fra::Renderer> renderer, std::shared_ptr<fra::MeshPool> meshPool,
                 std::shared_ptr<OctreeSystem> octreeSystem) :
        mRenderer(renderer),
        mMeshPool(meshPool), mOctreeSystem(octreeSystem)
    {
    }

    void PostUpdate(float dt) override;

  private:
    friend class SpaceApp;

    std::shared_ptr<fra::Renderer> mRenderer;
    std::shared_ptr<fra::MeshPool> mMeshPool;
    std::shared_ptr<OctreeSystem>  mOctreeSystem;

    std::shared_ptr<fra::Buffer> mInstanceMatrixBuffers;
    std::vector<std::uint32_t>   mRedShipModel;
    std::vector<std::uint32_t>   mBlueShipModel;
};