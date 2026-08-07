#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

class SpaceApp final : public fra::AbstractApplication
{
  public:
    SpaceApp(const skr::Arc<skr::ServiceProvider>& serviceProvider,
             const skr::Arc<fr::Registry>& registry) :
        AbstractApplication(serviceProvider), mRegistry(registry)
    {
    }

    void StartUp() override;
    void Update() override;
    void ShutDown() override;

  private:
    skr::Arc<fr::Registry> mRegistry;
};