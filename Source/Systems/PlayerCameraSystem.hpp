#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

#include "Components/PlayerComponent.hpp"

class PlayerCameraSystem final : public fr::System
{
  public:
    PlayerCameraSystem(const Ref<fr::Scene>&     scene,
                       const Ref<fra::Renderer>& renderer,
                       const Ref<fra::Window>&   window) :
        System(scene), mRenderer(renderer), mWindow(window)
    {
        mPlayer = mScene->FindUnique<PlayerComponent>();
    }

    void PostUpdate(float deltaTime) override;

  private:
    Ref<fra::Renderer> mRenderer;
    Ref<fra::Window>   mWindow;
    fr::Entity         mPlayer;
};