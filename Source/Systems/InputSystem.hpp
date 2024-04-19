#pragma once

#include <Freyr.hpp>
#include <Core/Window.hpp>
#include <Core/Renderer.hpp>

class InputSystem : public fr::System
{
  public:
    void PreUpdate(float deltaTime) override;
  private:
    friend class SpaceApp;
    std::shared_ptr<fra::Renderer> mRenderer;
    std::shared_ptr<fra::Window> mWindow; 
};