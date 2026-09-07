#include "LaserGunSystem.hpp"

#include "Components/AIControlledComponent.hpp"
#include "Components/DecayComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/LaserGunComponent.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/SquadComponent.hpp"
#include "Components/TransformComponent.hpp"

namespace
{
constexpr float BulletDamage = 50.0f;
} // namespace

void LaserGunSystem::Update(float deltaTime)
{
    mRegistry->CreateMutation()->EachAsync(
        [this, deltaTime](fr::Entity entity, LaserGunComponent& laserGun, TransformComponent& transform,
                          RigidBodyComponent& rigidBody, SquadComponent& squad) {
            laserGun.energySpent -= laserGun.energyCost * laserGun.energyCost * deltaTime;

            if (laserGun.energySpent < 0.0f)
                laserGun.energySpent = 0.0f;

            if (!laserGun.triggered)
                return;

            if (laserGun.fireTime < laserGun.fireRate)
            {
                laserGun.fireTime += deltaTime;
                return;
            }

            laserGun.fireTime = 0.0f;

            if (laserGun.energySpent + laserGun.energyCost > laserGun.maxEnergy)
                return;

            laserGun.energySpent += laserGun.energyCost;

            const auto forwardOffset = transform.GetForwardDirection() * 9.5f;
            const auto sideOffset    = transform.GetRightDirection() * 3.5f;
            const auto upOffset      = -transform.GetUpDirection() * 1.5f;

            const auto leftOffset  = forwardOffset - sideOffset + upOffset;
            const auto rightOffset = forwardOffset + sideOffset + upOffset;

            const auto material = squad.squad == Squad::Ally ? mAssetManager->GetBulletMaterial()
                                                            : mAssetManager->GetEnemyBulletMaterial();

            Shoot(entity, material, transform.position + leftOffset, transform.rotation,
                  rigidBody.velocity + transform.GetForwardDirection() * 500.f);
            Shoot(entity, material, transform.position + rightOffset, transform.rotation,
                  rigidBody.velocity + transform.GetForwardDirection() * 500.f);
        });
}

void LaserGunSystem::ConvertShip(fr::Entity target, Squad newSquad) const
{
    mRegistry->TryGetComponents<ModelComponent, SquadComponent, HealthComponent>(
        target, [&](ModelComponent& model, SquadComponent& targetSquad, HealthComponent& health) {
            targetSquad.squad = newSquad;

            if (newSquad == Squad::Ally)
            {
                model.meshes   = &mAssetManager->GetPlayerShipModel();
                model.material = mAssetManager->GetPlayerShipMaterial();
            }
            else
            {
                model.meshes   = &mAssetManager->GetEnemyShipModel();
                model.material = mAssetManager->GetEnemyShipMaterial();
            }

            health.hitPoints = health.maxHitPoints;
        });

    mRegistry->TryGetComponents<AIControlledComponent, LaserGunComponent>(
        target, [&](AIControlledComponent& aiControlled, LaserGunComponent& laserGun) {
            aiControlled.behaviour   = Behaviour::Patrol;
            aiControlled.target      = 0;
            aiControlled.frustration = 0.0f;
            laserGun.triggered       = false;
        });
}

void LaserGunSystem::ShatterRock(fr::Entity target, const HealthComponent& targetHealth) const
{
    mRegistry->TryGetComponents<TransformComponent, RigidBodyComponent, SphereColliderComponent>(
        target, [&](const TransformComponent& transform, const RigidBodyComponent& rigidBody,
                    SphereColliderComponent& sphereCollider) {
            const auto count   = sphereCollider.radius;
            const auto maxSize = sphereCollider.radius / 10.0f;

            mRegistry->CreateArchetypeBuilder()
                .WithComponent(ModelComponent { .meshes   = &mAssetManager->GetRock1Model(),
                                                .material = mAssetManager->GetRock1Material() })
                .WithComponent(TransformComponent { .position = transform.position, .rotation = glm::vec3(0.0) })
                .WithComponent(SphereColliderComponent { .radius = 1.0f, .offset = glm::vec3(0) })
                .WithComponent(RigidBodyComponent {
                    .isKinematic = false, .kinematicIfStop = true, .mass = rigidBody.mass / count })
                .WithComponent(HealthComponent { .hitPoints    = targetHealth.maxHitPoints / count,
                                                 .maxHitPoints = targetHealth.maxHitPoints / count })
                .ForEach<TransformComponent, RigidBodyComponent, SphereColliderComponent, ModelComponent>(
                    [this, velocity = rigidBody.mass / count, radius = sphereCollider.radius, maxSize](
                        auto entity, TransformComponent& rockTransform, RigidBodyComponent& rockRigidBody,
                        SphereColliderComponent& rockSphereCollider, ModelComponent& model) {
                        if (entity & 2)
                        {
                            model.meshes   = &mAssetManager->GetRock2Model();
                            model.material = mAssetManager->GetRock2Material();
                        }

                        rockTransform.position =
                            mRandom->PositionFrom(rockTransform.position, -radius, radius);
                        rockTransform.scale       = glm::vec3(mRandom->Float(1.0f, maxSize));
                        rockRigidBody.velocity    = mRandom->Position(-velocity, velocity);
                        rockSphereCollider.radius = rockTransform.scale.x;
                    })
                .WithEntities(static_cast<fr::Entity>(count))
                .Build();
        });

    mOctreeSystem->Remove(target);
    mRegistry->DestroyEntity(target);
}

void LaserGunSystem::OnCollision(const CollisionEvent& event) const
{
    mRegistry->TryGetComponents<BulletComponent>(event.collisor, [&](const BulletComponent& bullet) {
        mRegistry->DestroyEntity(event.collisor);

        const auto targetIsBullet =
            mRegistry->TryGetComponents<BulletComponent>(event.target, [&](const BulletComponent& otherBullet) {
                if (otherBullet.owner == bullet.owner)
                    return;

                mRegistry->DestroyEntity(event.target);
            });

        if (targetIsBullet)
            return;

        const auto ownerHasSquad = mRegistry->HasComponent<SquadComponent>(bullet.owner);
        if (!ownerHasSquad)
            return;

        Squad ownerSquad = Squad::Ally;
        mRegistry->TryGetComponents<SquadComponent>(bullet.owner, [&](const SquadComponent& squad) {
            ownerSquad = squad.squad;
        });

        const auto targetIsShip = mRegistry->HasComponent<SquadComponent>(event.target);
        if (targetIsShip)
        {
            bool friendlyFire = false;
            mRegistry->TryGetComponents<SquadComponent>(event.target, [&](const SquadComponent& targetSquad) {
                friendlyFire = targetSquad.squad == ownerSquad;
            });

            if (friendlyFire)
                return;

            mRegistry->TryGetComponents<AIControlledComponent, LaserGunComponent>(
                bullet.owner, [&](AIControlledComponent& aiControlled, LaserGunComponent& laserGun) {
                    aiControlled.behaviour   = Behaviour::Flee;
                    aiControlled.fleeTime    = 1.5f;
                    aiControlled.target      = event.target;
                    aiControlled.frustration = 0.0f;
                    laserGun.triggered       = false;
                });

            mRegistry->TryGetComponents<AIControlledComponent, LaserGunComponent>(
                event.target, [&](AIControlledComponent& aiControlled, LaserGunComponent& laserGun) {
                    aiControlled.behaviour   = Behaviour::Flee;
                    aiControlled.fleeTime    = 3.0f;
                    aiControlled.target      = bullet.owner;
                    aiControlled.frustration = 0.0f;
                    laserGun.triggered       = false;
                });

            mRegistry->TryGetComponents<HealthComponent>(event.target, [&](HealthComponent& targetHealth) {
                targetHealth.hitPoints -= BulletDamage;

                if (targetHealth.hitPoints > 0.0f)
                    return;

                // Player keeps their squad; AI ships flip allegiance when depleted.
                if (mRegistry->HasComponent<PlayerComponent>(event.target))
                {
                    targetHealth.hitPoints = targetHealth.maxHitPoints;
                    return;
                }

                ConvertShip(event.target, ownerSquad);
            });

            return;
        }

        mRegistry->TryGetComponents<HealthComponent>(event.target, [&](HealthComponent& targetHealth) {
            targetHealth.hitPoints -= BulletDamage;

            if (targetHealth.hitPoints <= 0.0f)
                ShatterRock(event.target, targetHealth);
        });
    });
}

void LaserGunSystem::Shoot(fr::Entity owner, std::uint32_t material, glm::vec3 position, glm::quat rotation,
                           glm::vec3 velocity)
{
    mRegistry->CreateEntity(
        BulletComponent { .owner = owner },
        TransformComponent { .position = position, .rotation = rotation, .scale = glm::vec3(1.0) },
        SphereColliderComponent { .radius = 1.0f },
        ModelComponent { .meshes = &mAssetManager->GetBulletModel(), .material = material },
        DecayComponent { .timeToLive = 1.0f },
        RigidBodyComponent {
            .mass     = 0.0001f,
            .velocity = velocity,
        });
}
