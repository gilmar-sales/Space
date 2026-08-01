#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

#include <AssetManager.hpp>

class SpaceApp final : public fra::AbstractApplication
{
  public:
    SpaceApp(const skr::Arc<skr::ServiceProvider>& serviceProvider,
             const skr::Arc<fr::Registry>& registry,
             const skr::Arc<AssetManager>&         assetManager) :
        AbstractApplication(serviceProvider), mRegistry(registry), mAssetManager(assetManager)
    {
    }

    void StartUp() override;
    void Update() override;
    void ShutDown() override;

  private:
    skr::Arc<fr::Registry>    mRegistry;
    skr::Arc<AssetManager> mAssetManager;
};