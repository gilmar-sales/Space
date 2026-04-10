#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

#include <AssetManager.hpp>

class SpaceApp final : public fra::AbstractApplication
{
  public:
    SpaceApp(const Ref<skr::ServiceProvider>& serviceProvider,
             const Ref<fr::Scene>&            scene,
             const Ref<AssetManager>&         assetManager) :
        AbstractApplication(serviceProvider), mScene(scene), mAssetManager(assetManager)
    {
    }

    void StartUp() override;
    void Update() override;
    void ShutDown() override;

  private:
    Ref<fr::Scene>    mScene;
    Ref<AssetManager> mAssetManager;
};