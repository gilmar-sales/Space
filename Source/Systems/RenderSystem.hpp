#pragma once

#include <Core/Renderer.hpp>
#include <Freyr.hpp>

class RenderSystem : public fr::System
{
    public:
        void Start() override;
        void Update(float dt) override;
        void PostUpdate(float dt) override;
    private:
        friend class SpaceApp;
        std::shared_ptr<fra::Renderer> mRenderer;
        std::vector<std::shared_ptr<fra::Buffer>> mInstanceBuffers;

        std::vector<std::uint32_t> mRedShipModel;
        std::vector<std::uint32_t> mBlueShipModel;
};