#include "LaserGunSystem.hpp"

#include "Components/DecayComponent.hpp"
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
                scene->CreateEntity(
                    [scene, octreeSystem](auto bullet) {
                        scene->TryGetComponents<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
                            bullet,
                            [&](TransformComponent&      transform,
                                SphereColliderComponent& sphereCollider,
                                RigidBodyComponent&      rigidBody) {
                                const auto particle = Particle { .entity         = bullet,
                                                                 .transform      = &transform,
                                                                 .sphereCollider = &sphereCollider };

                                auto octree      = octreeSystem->GetOctree()->Insert(particle);
                                rigidBody.octree = octree;
                            });
                    },
                    BulletComponent { .owner = entity },
                    TransformComponent { .position = transform.position + transform.GetForwardDirection() * 6.0f,
                                         .rotation = glm::vec3(0.0),
                                         .scale    = glm::vec3(1.0) },
                    SphereColliderComponent { .radius = 1.0f },
                    ModelComponent { .meshes = bulletMeshes, .material = bulletMaterial },
                    DecayComponent { .timeToLive = 2.0f },
                    RigidBodyComponent { .mass        = 0.0f,
                                         .velocity    = rigidBody.velocity + transform.GetForwardDirection() * 2000.f,
                                         .isKinematic = false,
                                         .octree      = nullptr });
            }
        });
}

void LaserGunSystem::OnCollision(const CollisionEvent& event)
{
    auto isBullet = mScene->HasComponent<BulletComponent>(event.collisor);

    if (!isBullet)
        return;

    mScene->DestroyEntity(event.collisor);
}