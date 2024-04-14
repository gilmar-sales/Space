#pragma once

#include <Core/AbstractApplication.hpp>
#include <Freyr.hpp>

#include "Systems/SpawnSystem.hpp"

class SpaceApp : public fra::AbstractApplication
{
public:
    void Startup() override
    {
        mManager = std::make_shared<fr::ECSManager>(2048);

        mManager->RegisterSystem<SpawnSystem>();
    }

    void Update() override
    {
        mManager->Update(mWindow->GetDeltaTime());
    }

private:
    std::shared_ptr<fr::ECSManager> mManager;
};