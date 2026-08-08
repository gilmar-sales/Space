#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

class InputSystem final : public fr::System
{
  public:
    InputSystem(const skr::Arc<fr::Registry>& registry,
                const skr::Arc<fra::Renderer>&     renderer,
                const skr::Arc<fra::Window>&       window,
                const skr::Arc<fra::EventManager>& eventManager) : System(registry), mRenderer(renderer), mWindow(window)
    {
        eventManager->Subscribe<fra::KeyPressedEvent>([this](const fra::KeyPressedEvent& event) {
            if (event.key == fra::KeyCode::M)
            {
                mMouseGrab = !mMouseGrab;

                mWindow->SetMouseGrab(mMouseGrab);
            }

            if (event.key == fra::KeyCode::V)
            {
                mRenderer->SetVSync(!mRenderer->GetVSync());
            }

            if (event.key == fra::KeyCode::Escape)
            {
                mWindow->Close();
            }

            if (event.key == fra::KeyCode::F11)
            {
                mWindow->SetFullscreen(!mWindow->IsFullscreen());
            }

            if (event.key == fra::KeyCode::F1)
            {
                if (!mProfilingStarted)
                {
                    mRegistry->BeginProfiling();
                    mProfilingStarted = true;
                }
                else
                {
                    mRegistry->EndProfiling();
                    mProfilingStarted = false;
                }
            }

            if (event.key == fra::KeyCode::F5)
            {
                CycleShadowQuality();
            }

            if (event.key == fra::KeyCode::F6)
            {
                CycleSsaoQuality();
            }

            if (event.key == fra::KeyCode::F7)
            {
                CycleTaaQuality();
            }

            if (event.key == fra::KeyCode::F8)
            {
                CycleBloomQuality();
            }
        });
    }

    void PreUpdate(float deltaTime) override;

  private:
    friend class SpaceApp;

    void CycleShadowQuality();
    void CycleSsaoQuality();
    void CycleTaaQuality();
    void CycleBloomQuality();

    bool mMouseGrab        = false;
    bool mProfilingStarted = false;

    skr::Arc<fra::Renderer> mRenderer;
    skr::Arc<fra::Window>   mWindow;
};
