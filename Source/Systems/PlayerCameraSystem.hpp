#pragma once

#include <Core/Renderer.hpp>
#include <Core/Window.hpp>
#include <Freyr/Freyr.hpp>

class PlayerCameraSystem : public fr::System
{
  public:
    PlayerCameraSystem(std::shared_ptr<fra::Renderer> renderer,
                       std::shared_ptr<fra::Window>
                           window) :
        mRenderer(renderer),
        mWindow(window)
    {
    }

    void PostUpdate(float deltaTime) override;

  private:
    std::shared_ptr<fra::Renderer> mRenderer;
    std::shared_ptr<fra::Window>   mWindow;
};