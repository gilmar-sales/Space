#include "AIControlSystem.hpp"

#include "Components/SpaceShipControlComponent.hpp"

constexpr auto CHASE_DISTANCE       = 450.0f;
constexpr auto SHOOT_DISTANCE       = 100.0f;
const auto     SHOOT_ANGLE_TRESHOLD = glm::cos(glm::radians(20.0f));

AIControlSystem::AIControlSystem(const Ref<fr::Scene>& scene, const Ref<OctreeSystem>& octreeSystem) :
    System(scene), mOctree(octreeSystem)
{
}

void AIControlSystem::Update(float deltaTime)
{
    mScene->CreateQuery()
        ->EachAsync<AIControlledComponent, TransformComponent, SquadComponent, LaserGunComponent,
                    SpaceShipControlComponent>(
            [this, deltaTime](fr::Entity entity, AIControlledComponent& aiControlled, TransformComponent& transform,
                              SquadComponent& squad, LaserGunComponent& laserGun,
                              SpaceShipControlComponent& spaceShipControl) {
                spaceShipControl.boost       = Boost;
                spaceShipControl.boostFactor = 1.0f;
                switch (aiControlled.behaviour)
                {
                    case Behaviour::Patrol:
                        Patrol(entity, aiControlled, squad, transform);
                        break;
                    case Behaviour::Chase:
                        Chase(aiControlled, spaceShipControl, squad, deltaTime, transform, laserGun);
                        break;
                    case Behaviour::Flee:
                        Flee(aiControlled, deltaTime);
                        spaceShipControl.boostFactor = BoostFactor;
                        break;
                }
            });
}

void AIControlSystem::Patrol(fr::Entity entity, AIControlledComponent& aiControlled, SquadComponent& squad,
                             TransformComponent& transform)
{

    std::optional<Particle> target = std::nullopt;

    float percent = 0.1f;

    do
    {
        auto particle = Particle { .entity         = entity,
                                   .transform      = transform,
                                   .sphereCollider = { .radius = CHASE_DISTANCE * percent } };
        target        = mOctree->FindFirst(particle, [&](const Particle& other) {
            auto shouldTarget = false;

            mScene->TryGetComponents<SquadComponent>(other.entity, [&](SquadComponent& otherSquad) {
                shouldTarget = squad.squad != otherSquad.squad;
            });

            return shouldTarget;
        });

        percent += 0.1f;
    } while (percent < 1.0f);

    if (!target.has_value())
        return;

    aiControlled.target    = target.value().entity;
    aiControlled.behaviour = Behaviour::Chase;
}

void AIControlSystem::Chase(AIControlledComponent& aiControlled, SpaceShipControlComponent& spaceShipControl,
                            const SquadComponent& squad, const float deltaTime, TransformComponent& transform,
                            LaserGunComponent& laserGun) const
{
    mScene->TryGetComponents<TransformComponent, SquadComponent>(
        aiControlled.target, [&](TransformComponent& playerTransform, const SquadComponent& targetSquad) {
            if (targetSquad.squad == squad.squad)
            {

                aiControlled.behaviour = Behaviour::Patrol;
                aiControlled.target    = 0;
                laserGun.triggered     = false;
                return;
            }

            const auto distanceVector = playerTransform.position - transform.position;

            const auto distance = glm::length(distanceVector);

            if (distance > CHASE_DISTANCE)
            {
                aiControlled.behaviour = Behaviour::Patrol;
                aiControlled.target    = 0;
                laserGun.triggered     = false;
                return;
            }

            const auto toTarget = glm::normalize(distanceVector);

            if (distance <= SHOOT_DISTANCE)
            {
                const auto dotProduct = glm::dot(transform.GetForwardDirection(), toTarget);

                laserGun.triggered = dotProduct > SHOOT_ANGLE_TRESHOLD;
            }

            const glm::vec3 torque = glm::normalize(glm::cross(transform.GetForwardDirection(), toTarget));

            constexpr auto threshold     = 0.1f;
            spaceShipControl.pitchTorque = std::abs(torque.x) > threshold ? torque.x : 0.0f;
            spaceShipControl.yawTorque   = std::abs(torque.y) > threshold ? torque.y : 0.0f;
            spaceShipControl.rollTorque  = std::abs(torque.z) > threshold ? torque.z * 0.4f : 0.0f;
        });
}
