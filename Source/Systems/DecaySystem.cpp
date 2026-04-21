#include "DecaySystem.hpp"

#include "Components/DecayComponent.hpp"

void DecaySystem::PreUpdate(float deltaTime)
{
    mScene->CreateQuery()->EachAsync<DecayComponent>([this, deltaTime](auto entity, DecayComponent& decay) {
        decay.timeToLive -= deltaTime;

        if (decay.timeToLive <= 0)
        {
            mScene->DestroyEntity(entity);
        }
    });
}