#pragma once

#include <Core/Renderer.hpp>
#include <Core/Window.hpp>
#include <Freyr/Freyr.hpp>

class PlayerCameraSystem final : public fr::System
{
  public:
    PlayerCameraSystem(const std::shared_ptr<fr::Scene>&     scene,
                       const std::shared_ptr<fra::Renderer>& renderer,
                       const std::shared_ptr<fra::Window>&   window) :
        System(scene), mRenderer(renderer), mWindow(window)
    {
    }

    void PostUpdate(float deltaTime) override;

  private:
    std::shared_ptr<fra::Renderer> mRenderer;
    std::shared_ptr<fra::Window>   mWindow;
};