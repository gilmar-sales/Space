#include "LaserGunSystem.hpp"

#include "Components/DecayComponent.hpp"
#include "Components/EnemyComponent.hpp"
#include "Components/LaserGunComponent.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"

void LaserGunSystem::Update(float deltaTime)
{
    mScene->ForEachAsync<LaserGunComponent, TransformComponent, RigidBodyComponent>(
        [scene          = mScene,
         octreeSystem   = mOctreeSystem,
         bulletMeshes   = &mBulletModel,
         bulletMaterial = mBulletMaterial,
         deltaTime](auto                entity,
                    LaserGunComponent&  laserGun,
                    TransformComponent& transform,
                    RigidBodyComponent& rigidBody) {
            if (laserGun.triggered)
            {
                if (laserGun.fireTime < laserGun.fireRate)
                {
                    laserGun.fireTime += deltaTime;
                    return;
                }

                laserGun.fireTime = 0.0f;

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
                        DecayComponent { .timeToLive = 2.5f },
                        RigidBodyComponent { .mass     = 0.0f,
                                             .velocity = rigidBody.velocity + transform.GetForwardDirection() * 600.f,
                                             .isKinematic = false });
                };

                const auto leftOffset = transform.GetUpDirection() * 4.0f + transform.GetForwardDirection() * 5.5f -
                                        transform.GetRightDirection() * 3.5f;
                const auto rightOffset = transform.GetUpDirection() * 4.0f + transform.GetForwardDirection() * 5.5f +
                                         transform.GetRightDirection() * 3.5f;

                shoot(leftOffset);
                shoot(rightOffset);
            }
        });
}

void LaserGunSystem::OnCollision(const CollisionEvent& event) const
{
    mScene->TryGetComponents<BulletComponent>(event.collisor, [&](const BulletComponent& bullet) {
        if (event.target == bullet.owner)
            return;

        if (mScene->HasComponent<BulletComponent>(event.target))
            return;

        if (mScene->HasComponent<EnemyComponent>(event.target))
            mScene->DestroyEntity(event.target);

        mScene->DestroyEntity(event.collisor);
    });
}