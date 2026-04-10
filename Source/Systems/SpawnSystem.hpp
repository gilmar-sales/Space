#pragma once

#include <Freyr/Freyr.hpp>

#include <AssetManager.hpp>
#include <Random.hpp>

class SpawnSystem final : public fr::System
{
  public:
    explicit SpawnSystem(const Ref<fr::Scene>& scene, const Ref<AssetManager>& assetManager, const Ref<Random>& random);

  private:
    Ref<AssetManager> mAssetManager;
    Ref<Random>       mRandom;
};