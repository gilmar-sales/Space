#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

#include <Systems/OctreeSystem.hpp>

struct InstanceDraw
{
    size_t                      index;
    int                         instanceCount;
    std::vector<std::uint32_t>* meshes;
    std::uint32_t               material;
};

class RenderSystem final : public fr::System
{
  public:
    RenderSystem(const Ref<fr::Scene>&         scene,
                 const Ref<fra::Renderer>&     renderer,
                 const Ref<fra::MeshPool>&     meshPool,
                 const Ref<fra::MaterialPool>& materialPool,
                 const Ref<OctreeSystem>&      octreeSystem);

    void PostUpdate(float dt) override;

  private:
    friend class SpaceApp;

    std::vector<Particle>  mRenderables;
    std::vector<glm::mat4> mMatrices;

    Ref<fra::Renderer>     mRenderer;
    Ref<fra::MaterialPool> mMaterialPool;
    Ref<fra::MeshPool>     mMeshPool;
    Ref<OctreeSystem>      mOctreeSystem;

    Ref<fra::Buffer> mInstanceMatrixBuffers;
};