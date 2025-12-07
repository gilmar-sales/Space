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
                            TransformComponent&      transform,
                            SphereColliderComponent& sphereCollider,
                            ModelComponent&,
                            DecayComponent&,
                            RigidBodyComponent&) {
                            const auto particle = Particle { .entity         = bullet,
                                                             .transform      = &transform,
                                                             .sphereCollider = &sphereCollider };

                            octreeSystem->GetOctree()->Insert(particle);
                        },
                        BulletComponent { .owner = entity },
                        TransformComponent { .position = transform.position + offset,
                                             .rotation = glm::vec3(0.0),
                                             .scale    = glm::vec3(1.0) },
                        SphereColliderComponent { .radius = 1.0f },
                        ModelComponent { .meshes = bulletMeshes, .material = bulletMaterial },
                        DecayComponent { .timeToLive = 2.f },
                        RigidBodyComponent { .mass     = 0.0f,
                                             .velocity = rigidBody.velocity + transform.GetForwardDirection() * 1200.f,
                                             .isKinematic = false });
                };

                const auto leftOffset  = transform.GetForwardDirection() * 7.5f - transform.GetRightDirection() * 3.5f;
                const auto rightOffset = transform.GetForwardDirection() * 7.5f + transform.GetRightDirection() * 3.5f;

                shoot(leftOffset);
                shoot(rightOffset);
            }
        });
}

void LaserGunSystem::OnCollision(const CollisionEvent& event) const
{
    mScene->TryGetComponents<BulletComponent>(event.collisor, [&](const BulletComponent& bullet) {
        if (mScene->HasComponent<PlayerComponent>(event.target))
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
                    mScene->TryGetComponents<TransformComponent, RigidBodyComponent, SphereColliderComponent>(
                        event.target,
                        [&](const TransformComponent& transform,
                            const RigidBodyComponent& rigidBody,
                            SphereColliderComponent&  sphereCollider) {
                            if (rigidBody.mass < 10.0f)
                                return;

                            const auto count = sphereCollider.radius;
                            mScene->CreateArchetypeBuilder()
                                .WithComponent(ModelComponent { .meshes   = &mAssetManager->GetRock1Model(),
                                                                .material = mAssetManager->GetRock1Material() })
                                .WithComponent(
                                    TransformComponent { .position = transform.position, .rotation = glm::vec3(0.0) })
                                .WithComponent(SphereColliderComponent { .radius = 1.0f, .offset = glm::vec3(0) })
                                .WithComponent(
                                    RigidBodyComponent { .mass = rigidBody.mass / count, .isKinematic = true })
                                .WithComponent(HealthComponent { .hitPoints = 1000 / count })
                                .ForEach<TransformComponent, RigidBodyComponent, SphereColliderComponent>(
                                    [&, radius = sphereCollider.radius](auto,
                                                                        TransformComponent&      transform,
                                                                        RigidBodyComponent&      rigidBody,
                                                                        SphereColliderComponent& sphereCollider) {
                                        transform.position = mRandom->PositionFrom(transform.position, -radius, radius);
                                        transform.scale    = glm::vec3(mRandom->Float(5.0f, 20.0f));
                                        rigidBody.velocity = mRandom->Position(-radius, radius);
                                        sphereCollider.radius = transform.scale.x;
                                    })
                                .WithEntities(count)
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