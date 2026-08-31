#include "DecaySystem.hpp"

#include "Components/DecayComponent.hpp"

void DecaySystem::PreUpdate(float deltaTime)
{
    mRegistry->CreateMutation()->EachAsync([this, deltaTime](fr::Entity entity, DecayComponent& decay) {
        decay.timeToLive -= deltaTime;

        if (decay.timeToLive <= 0)
        {
            mRegistry->DestroyEntity(entity);
        }
    });
}