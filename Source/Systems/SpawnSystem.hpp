#pragma once

#include <Freyr/Freyr.hpp>

#include <AssetManager.hpp>
#include <Random.hpp>

class SpawnSystem final : public fr::System
{
  public:
    explicit SpawnSystem(const skr::Arc<fr::Registry>& registry, const skr::Arc<AssetManager>& assetManager, const skr::Arc<Random>& random);

  private:
    skr::Arc<AssetManager> mAssetManager;
    skr::Arc<Random>       mRandom;
};