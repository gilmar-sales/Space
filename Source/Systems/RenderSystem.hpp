#pragma once

#include <Core/Renderer.hpp>
#include <Freyr/Freyr.hpp>

#include <Systems/OctreeSystem.hpp>

class RenderSystem : public fr::System
{
  public:
    RenderSystem(std::shared_ptr<fra::Renderer> renderer, std::shared_ptr<OctreeSystem> octreeSystem) :
        mRenderer(renderer), mOctreeSystem(octreeSystem) {}

    void Start() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;

  private:
    friend class SpaceApp;

    std::shared_ptr<fra::Renderer> mRenderer;
    std::shared_ptr<OctreeSystem>  mOctreeSystem;

    std::vector<std::shared_ptr<fra::Buffer>> mInstanceBuffers;
    std::vector<std::uint32_t>                mRedShipModel;
    std::vector<std::uint32_t>                mBlueShipModel;
};