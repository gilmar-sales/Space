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
                 const Ref<fra::Window>&       window,
                 const Ref<fra::MeshPool>&     meshPool,
                 const Ref<fra::MaterialPool>& materialPool,
                 const Ref<OctreeSystem>&      octreeSystem);

    void PostUpdate(float dt) override;

  private:
    void BeginFrame() const;
    void DrawInstanced();
    void EndFrame() const;

    friend class SpaceApp;

    fr::Entity mPlayer;

    std::vector<std::vector<Particle>>  mRenderables;
    std::vector<std::vector<glm::mat4>> mMatrices;
    std::vector<Ref<fra::Buffer>>       mInstanceMatrixBuffers;

    Ref<fra::Renderer>     mRenderer;
    Ref<fra::Window>       mWindow;
    Ref<fra::MaterialPool> mMaterialPool;
    Ref<fra::MeshPool>     mMeshPool;
    Ref<OctreeSystem>      mOctreeSystem;
};