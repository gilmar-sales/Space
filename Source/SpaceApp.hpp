#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

class SpaceApp final : public fra::AbstractApplication
{
  public:
    SpaceApp(const Ref<skr::ServiceProvider>& serviceProvider,
             const Ref<fr::Scene>&            scene) :
        AbstractApplication(serviceProvider), mScene(scene)
    {
    }

    void StartUp() override;
    void Update() override;
    void ShutDown() override;

  private:
    Ref<fr::Scene> mScene;
};