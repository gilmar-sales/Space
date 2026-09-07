#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

#include <Systems/OctreeSystem.hpp>

#include <vector>

class RenderSystem final : public fr::System
{
  public:
    RenderSystem(const skr::Arc<fr::Registry>& registry, const skr::Arc<fra::Renderer>& renderer,
                 const skr::Arc<fra::Window>& window, const skr::Arc<fra::MeshPool>& meshPool,
                 const skr::Arc<fra::MaterialPool>& materialPool, const skr::Arc<OctreeSystem>& octreeSystem,
                 const skr::Arc<fr::ThreadPool>& threadPool, const skr::Arc<fra::EventManager>& eventManager);

    void PostUpdate(float dt) override;

  private:
    void BeginFrame();
    void SubmitScene();
    void SubmitHealthBars();
    void EndFrame() const;

    friend class SpaceApp;

    std::optional<fr::Entity> mPlayer;

    std::vector<Particle>                 mRenderables;
    std::vector<fra::SceneInstanceUpload> mUploads;
    glm::mat4                             mViewProj { 1.0f };

    skr::Arc<fra::Renderer>     mRenderer;
    skr::Arc<fra::Window>       mWindow;
    skr::Arc<fra::MaterialPool> mMaterialPool;
    skr::Arc<fra::MeshPool>     mMeshPool;
    skr::Arc<OctreeSystem>      mOctreeSystem;
    skr::Arc<fr::ThreadPool>    mThreadPool;
    bool                        mEnabled;
};
