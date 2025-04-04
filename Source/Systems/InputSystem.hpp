#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

class InputSystem final : public fr::System
{
  public:
    InputSystem(const Ref<fr::Scene>&         scene,
                const Ref<fra::Renderer>&     renderer,
                const Ref<fra::Window>&       window,
                const Ref<fra::EventManager>& eventManager) :
        System(scene), mRenderer(renderer), mWindow(window)
    {
        eventManager->Subscribe<fra::KeyPressedEvent>(
            [this](const fra::KeyPressedEvent& event) {
                if (event.key == fra::KeyCode::M)
                {
                    mMouseGrab = !mMouseGrab;

                    mWindow->SetMouseGrab(mMouseGrab);
                }

                if (event.key == fra::KeyCode::Escape)
                {
                    mWindow->Close();
                }

                if (event.key == fra::KeyCode::F11)
                {
                    mWindow->SetFullscreen(!mWindow->IsFullscreen());
                }

                if (event.key == fra::KeyCode::F6)
                {
                    if (!mProfilingStarted)
                    {
                        mScene->StartProfiling();
                        mProfilingStarted = true;
                    }
                    else
                    {
                        mScene->EndProfiling();
                        mProfilingStarted = false;
                    }
                }
            });
    }

    void PreUpdate(float deltaTime) override;

  private:
    friend class SpaceApp;

    bool mMouseGrab        = false;
    bool mProfilingStarted = false;

    Ref<fra::Renderer> mRenderer;
    Ref<fra::Window>   mWindow;
};