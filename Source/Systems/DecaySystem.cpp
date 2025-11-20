#include "DecaySystem.hpp"

#include "Components/DecayComponent.hpp"

void DecaySystem::PreFixedUpdate(float deltaTime)
{
    mScene->ForEachAsync<DecayComponent>([deltaTime, scene = mScene](auto entity, DecayComponent& decay) {
        decay.timeToLive -= deltaTime;

        if (decay.timeToLive <= 0)
        {
            scene->DestroyEntity(entity);
        }
    });
}