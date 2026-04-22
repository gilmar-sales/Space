#include "LaserGunSystem.hpp"

#include "Components/AIControlledComponent.hpp"
#include "Components/DecayComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/LaserGunComponent.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/SquadComponent.hpp"
#include "Components/TransformComponent.hpp"

void LaserGunSystem::Update(float deltaTime) {
    mScene->CreateQuery()->EachAsync<LaserGunComponent, TransformComponent, RigidBodyComponent>(
        [this, deltaTime](auto entity,
                          LaserGunComponent &laserGun,
                          TransformComponent &transform,
                          RigidBodyComponent &rigidBody) {
            laserGun.energySpent -= laserGun.energyCost * laserGun.energyCost * deltaTime;

            if (laserGun.energySpent < 0.0f)
                laserGun.energySpent = 0.0f;

            if (laserGun.triggered) {
                if (laserGun.fireTime < laserGun.fireRate) {
                    laserGun.fireTime += deltaTime;
                    return;
                }

                laserGun.fireTime = 0.0f;

                if (laserGun.energySpent + laserGun.energyCost > laserGun.maxEnergy) {
                    return;
                }

                laserGun.energySpent += laserGun.energyCost;

                const auto forwarOffset = transform.GetForwardDirection() * 9.5f;
                const auto sideOffset = transform.GetRightDirection() * 3.5f;
                const auto upOffset = transform.GetUpDirection() * 1.5f;

                const auto leftOffset = forwarOffset - sideOffset + upOffset;
                const auto rightOffset = forwarOffset + sideOffset + upOffset;

                Shoot(entity, transform.position + leftOffset,
                      rigidBody.velocity + transform.GetForwardDirection() * 500.f);
                Shoot(entity, transform.position + rightOffset,
                      rigidBody.velocity + transform.GetForwardDirection() * 500.f);
            }
        });
}

void LaserGunSystem::OnCollision(const CollisionEvent &event) const {
    mScene->TryGetComponents<BulletComponent>(event.collisor, [&](const BulletComponent &bullet) {
        mScene->DestroyEntity(event.collisor);
        const auto targetIsBullet = mScene->TryGetComponents<BulletComponent>(
            event.target, [&](const BulletComponent &otherBullet) {
                if (otherBullet.owner == bullet.owner)
                    return;

                mScene->DestroyEntity(event.target);
            });

        if (targetIsBullet)
            return;

        mScene->TryGetComponents<SquadComponent>(bullet.owner, [&](const SquadComponent &onwerSquad) {
            mScene->TryGetComponents<AIControlledComponent, LaserGunComponent>(
                bullet.owner,
                [](AIControlledComponent &aiControlled, LaserGunComponent &laserGun) {
                    aiControlled.behaviour = Behaviour::Patrol;
                    aiControlled.target = 0;
                    laserGun.triggered = false;
                });

            mScene->TryGetComponents<AIControlledComponent, LaserGunComponent>(
                event.target,
                [](AIControlledComponent &aiControlled, LaserGunComponent &laserGun) {
                    aiControlled.behaviour = Behaviour::Patrol;
                    aiControlled.target = 0;
                    laserGun.triggered = false;
                });

            mScene->TryGetComponents<ModelComponent, SquadComponent>(
                event.target,
                [&](ModelComponent &model, SquadComponent &targetSquad) {
                    if (onwerSquad.squad == Squad::Ally) {
                        model.meshes = &mAssetManager->GetPlayerShipModel();
                        model.material = mAssetManager->GetPlayerShipMaterial();
                        targetSquad.squad = Squad::Ally;
                    } else {
                        model.meshes = &mAssetManager->GetEnemyShipModel();
                        model.material = mAssetManager->GetEnemyShipMaterial();
                        targetSquad.squad = Squad::Enemy;
                    }
                });
        });

        if (event.target == mPlayer)
            return;

        mScene->TryGetComponents<HealthComponent>(event.target, [&](HealthComponent &targetHealth) {
            targetHealth.hitPoints -= 50.0f;

            if (mScene->HasComponent<SquadComponent>(bullet.owner))
                return;

            if (targetHealth.hitPoints <= 0.0f) {
                if (!mScene->HasComponent<SquadComponent>(event.target))
                    mScene->TryGetComponents<TransformComponent, RigidBodyComponent, SphereColliderComponent>(
                        event.target,
                        [&](const TransformComponent &transform,
                            const RigidBodyComponent &rigidBody,
                            SphereColliderComponent &sphereCollider) {
                            const auto count = sphereCollider.radius;
                            const auto maxSize = sphereCollider.radius / 10.0f;

                            mScene->CreateArchetypeBuilder()
                                    .WithComponent(ModelComponent{
                                        .meshes = &mAssetManager->GetRock1Model(),
                                        .material = mAssetManager->GetRock1Material()
                                    })
                                    .WithComponent(TransformComponent{
                                        .position = transform.position,
                                        .rotation = glm::vec3(0.0)
                                    })
                                    .WithComponent(
                                        SphereColliderComponent{.radius = 1.0f, .offset = glm::vec3(0)})
                                    .WithComponent(RigidBodyComponent{
                                        .isKinematic = false,
                                        .kinematicIfStop = true,
                                        .mass = rigidBody.mass / count
                                    })
                                    .WithComponent(HealthComponent{.hitPoints = targetHealth.hitPoints / count})
                                    .ForEach<TransformComponent,
                                        RigidBodyComponent,
                                        SphereColliderComponent,
                                        ModelComponent>(
                                        [this,
                                            velocity = rigidBody.mass / count,
                                            radius = sphereCollider.radius,
                                            maxSize](auto entity,
                                                     TransformComponent &rockTransform,
                                                     RigidBodyComponent &rockRigidBody,
                                                     SphereColliderComponent &rockSphereCollider,
                                                     ModelComponent &model) {
                                            if (entity & 2) {
                                                model.meshes = &mAssetManager->GetRock2Model();
                                                model.material = mAssetManager->GetRock2Material();
                                            }

                                            rockTransform.position =
                                                    mRandom->PositionFrom(
                                                        rockTransform.position, -radius, radius);
                                            rockTransform.scale = glm::vec3(mRandom->Float(1.0f, maxSize));
                                            rockRigidBody.velocity = mRandom->Position(-velocity, velocity);
                                            rockSphereCollider.radius = rockTransform.scale.x;
                                        })
                                    .WithEntities(static_cast<fr::Entity>(count))
                                    .Build();
                        });

                mOctreeSystem->Remove(event.target);
                mScene->DestroyEntity(event.target);
            }
        });
    });
}

void LaserGunSystem::Shoot(fr::Entity owner, glm::vec3 position, glm::vec3 velocity) {
    mScene->CreateEntity(
        BulletComponent{.owner = owner},
        TransformComponent{
            .position = position,
            .rotation = glm::vec3(0.0),
            .scale = glm::vec3(1.0)
        },
        SphereColliderComponent{.radius = 1.0f},
        ModelComponent{
            .meshes = &mAssetManager->GetBulletModel(),
            .material = mAssetManager->GetBulletMaterial()
        },
        DecayComponent{.timeToLive = 1.0f},
        RigidBodyComponent{
            .mass = 0.0001f,
            .velocity = velocity,
        });
}
