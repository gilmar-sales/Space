#include "LaserGunSystem.hpp"

#include "Components/DecayComponent.hpp"
#include "Components/EnemyComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/LaserGunComponent.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"

void LaserGunSystem::Update(float deltaTime)
{
    mScene->ForEachAsync<LaserGunComponent, TransformComponent, RigidBodyComponent>(
        [scene          = mScene,
         octreeSystem   = mOctreeSystem,
         bulletMeshes   = &mAssetManager->GetBulletModel(),
         bulletMaterial = mAssetManager->GetBulletMaterial(),
         deltaTime](auto                entity,
                    LaserGunComponent&  laserGun,
                    TransformComponent& transform,
                    RigidBodyComponent& rigidBody) {
            laserGun.energySpent -= laserGun.energyCost * laserGun.energyCost * deltaTime;

            if (laserGun.energySpent < 0.0f)
                laserGun.energySpent = 0.0f;

            if (laserGun.triggered)
            {
                if (laserGun.fireTime < laserGun.fireRate)
                {
                    laserGun.fireTime += deltaTime;
                    return;
                }

                laserGun.fireTime = 0.0f;

                if (laserGun.energySpent + laserGun.energyCost > laserGun.maxEnergy)
                {
                    return;
                }

                laserGun.energySpent += laserGun.energyCost;

                auto shoot = [&](glm::vec3 offset) {
                    scene->CreateEntity(
                        [scene, octreeSystem](
                            auto bullet,
                            BulletComponent&,
                            const TransformComponent&      transform,
                            const SphereColliderComponent& sphereCollider,
                            ModelComponent&,
                            DecayComponent&,
                            RigidBodyComponent&) {
                            const auto particle = Particle { .entity         = bullet,
                                                             .transform      = transform,
                                                             .sphereCollider = sphereCollider };

                            octreeSystem->GetOctree()->Insert(particle);
                        },
                        BulletComponent { .owner = entity },
                        TransformComponent { .position = transform.position + offset,
                                             .rotation = glm::vec3(0.0),
                                             .scale    = glm::vec3(1.0) },
                        SphereColliderComponent { .radius = 1.0f },
                        ModelComponent { .meshes = bulletMeshes, .material = bulletMaterial },
                        DecayComponent { .timeToLive = 3.5f },
                        RigidBodyComponent {
                            .mass     = 0.0f,
                            .velocity = rigidBody.velocity + transform.GetForwardDirection() * 800.f,
                        });
                };

                const auto forwarOffset = transform.GetForwardDirection() * 9.5f;
                const auto sideOffset   = transform.GetRightDirection() * 3.5f;
                const auto upOffset     = transform.GetUpDirection() * 1.5f;

                const auto leftOffset  = forwarOffset - sideOffset + upOffset;
                const auto rightOffset = forwarOffset + sideOffset + upOffset;

                shoot(leftOffset);
                shoot(rightOffset);
            }
        });
}

void LaserGunSystem::OnCollision(const CollisionEvent& event) const
{
    mScene->TryGetComponents<BulletComponent>(event.collisor, [&](const BulletComponent& bullet) {
        if (event.target == mPlayer || event.collisor == mPlayer)
            return;

        if (!mScene->TryGetComponents<BulletComponent>(event.target, [&](const BulletComponent& otherBullet) {
                if (otherBullet.owner == bullet.owner)
                    return;

                mScene->DestroyEntity(event.target);
            }))
        {
            mScene->TryGetComponents<HealthComponent>(event.target, [&](HealthComponent& targetHealth) {
                targetHealth.hitPoints -= 50.0f;

                if (targetHealth.hitPoints <= 0.0f)
                {
                    if (!mScene->HasComponent<EnemyComponent>(event.target))
                        mScene->TryGetComponents<TransformComponent, RigidBodyComponent, SphereColliderComponent>(
                            event.target,
                            [&](const TransformComponent& transform,
                                const RigidBodyComponent& rigidBody,
                                SphereColliderComponent&  sphereCollider) {
                                const auto count   = sphereCollider.radius;
                                const auto maxSize = sphereCollider.radius / 10.0f;

                                mScene->CreateArchetypeBuilder()
                                    .WithComponent(ModelComponent { .meshes   = &mAssetManager->GetRock1Model(),
                                                                    .material = mAssetManager->GetRock1Material() })
                                    .WithComponent(TransformComponent { .position = transform.position,
                                                                        .rotation = glm::vec3(0.0) })
                                    .WithComponent(SphereColliderComponent { .radius = 1.0f, .offset = glm::vec3(0) })
                                    .WithComponent(
                                        RigidBodyComponent { .isKinematic = false, .mass = rigidBody.mass / count })
                                    .WithComponent(HealthComponent { .hitPoints = targetHealth.hitPoints / count })
                                    .ForEach<TransformComponent, RigidBodyComponent, SphereColliderComponent>(
                                        [this,
                                         velocity = rigidBody.mass / count,
                                         radius   = sphereCollider.radius,
                                         maxSize](auto,
                                                  TransformComponent&      rockTransform,
                                                  RigidBodyComponent&      rockRigidBody,
                                                  SphereColliderComponent& rockSphereCollider) {
                                            rockTransform.position =
                                                mRandom->PositionFrom(rockTransform.position, -radius, radius);
                                            rockTransform.scale       = glm::vec3(mRandom->Float(1.0f, maxSize));
                                            rockRigidBody.velocity    = mRandom->Position(-velocity, velocity);
                                            rockSphereCollider.radius = rockTransform.scale.x;
                                        })
                                    .WithEntities(static_cast<fr::Entity>(count))
                                    .Build();
                            });
                    mScene->DestroyEntity(event.target);
                }
            });

            if (mScene->HasComponent<EnemyComponent>(event.target) &&
                mScene->HasComponent<PlayerComponent>(bullet.owner))
                mScene->DestroyEntity(event.target);

            if (event.target != bullet.owner)
                mScene->DestroyEntity(event.collisor);
        }
    });
}