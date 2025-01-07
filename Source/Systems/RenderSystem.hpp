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

class RenderSystem final : public fr::System
{
  public:
    RenderSystem(const std::shared_ptr<fr::Scene>&        scene,
                 const std::shared_ptr<fra::Renderer>&    renderer,
                 const std::shared_ptr<fra::MeshPool>&    meshPool,
                 const std::shared_ptr<fra::TexturePool>& texturePool,
                 const std::shared_ptr<OctreeSystem>&     octreeSystem);

    void PostUpdate(float dt) override;

  private:
    friend class SpaceApp;

    std::shared_ptr<fra::Renderer>    mRenderer;
    std::shared_ptr<fra::TexturePool> mTexturePool;
    std::shared_ptr<fra::MeshPool>    mMeshPool;
    std::shared_ptr<OctreeSystem>     mOctreeSystem;

    std::shared_ptr<fra::Buffer> mInstanceMatrixBuffers;

    std::vector<std::uint32_t> mRedShipModel;
    std::vector<std::uint32_t> mBlueShipModel;
    std::vector<std::uint32_t> mCubeModel;
    std::uint32_t mBlankTexture;
};