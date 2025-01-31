#pragma once

#include <Core/AbstractApplication.hpp>
#include <Freyr/Freyr.hpp>

class SpaceApp final : public fra::AbstractApplication
{
  public:
    SpaceApp(const Ref<ServiceProvider>&       serviceProvider,
             const std::shared_ptr<fr::Scene>& scene) :
        AbstractApplication(serviceProvider), mScene(scene)
    {
    }

    void StartUp() override;
    void Update() override;
    void ShutDown() override;

  private:
    std::shared_ptr<fr::Scene> mScene;
};