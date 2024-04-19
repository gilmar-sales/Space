#pragma once

#include <Core/Renderer.hpp>
#include <Core/Window.hpp>
#include <Freyr.hpp>


class InputSystem : public fr::System
{
  public:
    void PreUpdate(float deltaTime) override;

  private:
    friend class SpaceApp;
    std::shared_ptr<fra::Renderer> mRenderer;
    std::shared_ptr<fra::Window>   mWindow;
};