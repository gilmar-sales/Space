#pragma once

#include "Events/KeyDownEvent.hpp"

#include <Core/Renderer.hpp>
#include <Core/Window.hpp>
#include <Freyr/Freyr.hpp>

class InputSystem final : public fr::System
{
  public:
    InputSystem(const std::shared_ptr<fr::Scene>&         scene,
                const std::shared_ptr<fra::Renderer>&     renderer,
                const std::shared_ptr<fra::Window>&       window,
                const std::shared_ptr<fra::EventManager>& eventManager) :
        System(scene), mRenderer(renderer), mWindow(window)
    {
        static bool grab = false;

        eventManager->Subscribe<fra::KeyPressedEvent>(
            [this](const fra::KeyPressedEvent& event) {
                if (event.key == fra::KeyCode::M)
                {
                    grab = !grab;

                    mWindow->SetMouseGrab(grab);
                }
            });
    }

    void PreUpdate(float deltaTime) override;

  private:
    friend class SpaceApp;

    std::shared_ptr<fra::Renderer> mRenderer;
    std::shared_ptr<fra::Window>   mWindow;
};