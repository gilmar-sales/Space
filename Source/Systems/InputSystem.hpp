#pragma once

#include <Core/Renderer.hpp>
#include <Core/Window.hpp>
#include <Freyr/Freyr.hpp>

class InputSystem : public fr::System
{
  public:
    InputSystem(std::shared_ptr<fra::Renderer> renderer,
                std::shared_ptr<fra::Window>
                    window) :
        mRenderer(renderer),
        mWindow(window)
    {
    }

    void PreUpdate(float deltaTime) override;

    static glm::vec3 cameraPosition;
    static glm::vec3 cameraForward;

  private:
    friend class SpaceApp;

    std::shared_ptr<fra::Renderer> mRenderer;
    std::shared_ptr<fra::Window>   mWindow;
};