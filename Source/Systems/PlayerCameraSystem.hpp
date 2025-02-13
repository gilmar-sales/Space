#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

#include "Components/PlayerComponent.hpp"

class PlayerCameraSystem final : public fr::System
{
  public:
    PlayerCameraSystem(const std::shared_ptr<fr::Scene>&     scene,
                       const std::shared_ptr<fra::Renderer>& renderer,
                       const std::shared_ptr<fra::Window>&   window) :
        System(scene), mRenderer(renderer), mWindow(window)
    {
        mPlayer = mScene->FindUnique<PlayerComponent>();
    }

    void PostUpdate(float deltaTime) override;

  private:
    std::shared_ptr<fra::Renderer> mRenderer;
    std::shared_ptr<fra::Window>   mWindow;
    fr::Entity                     mPlayer;
};