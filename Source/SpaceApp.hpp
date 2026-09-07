#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

class SpaceApp final : public fra::AbstractApplication
{
  public:
    SpaceApp(const skr::Arc<skr::ServiceProvider>& serviceProvider,
             const skr::Arc<fr::Registry>&         registry);

    void StartUp() override;
    void Update() override;
    void ShutDown() override;

  private:
    // Freya 0.44+: Window / Renderer / EventManager are window-scoped.
    // Freyr resolves systems from a fresh child of the root provider, so
    // construct Freya-bound systems from the main window scope first — their
    // singleton instances are shared via the root singleton cache.
    void WarmFreyaBoundSystems();

    skr::Arc<fr::Registry> mRegistry;
};
