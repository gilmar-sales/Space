#include "LaserGunSystem.hpp"

#include "Components/DecayComponent.hpp"
#include "Components/LaserGunComponent.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"

void LaserGunSystem::Update(float deltaTime)
{
    mScene->ForEachAsync<LaserGunComponent,
                         TransformComponent,
                         RigidBodyComponent>(
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
                auto bullet       = scene->CreateEntity();
                scene->AddComponent<BulletComponent>(bullet,
                                                     { .owner = entity });
                scene->AddComponent<TransformComponent>(
                    bullet,
                    { .position = transform.position +
                                  transform.GetForwardDirection() * 6.0f,
                      .rotation = glm::vec3(0.0),
                      .scale    = glm::vec3(1.0) });
                scene->AddComponent<SphereColliderComponent>(
                    bullet,
                    { .radius = 1.0f });
                scene->AddComponent<ModelComponent>(
                    bullet,
                    { .meshes = bulletMeshes, .material = bulletMaterial });
                scene->AddComponent(bullet,
                                    DecayComponent { .timeToLive = 2.0f });

                const auto particle = Particle {
                    .entity = bullet,
                    .transform =
                        &scene->GetComponent<TransformComponent>(bullet),
                    .sphereCollider =
                        &scene->GetComponent<SphereColliderComponent>(bullet)
                };

                auto octree = octreeSystem->GetOctree()->Insert(particle);
                scene->AddComponent<RigidBodyComponent>(
                    bullet,
                    { .mass     = 0.0f,
                      .velocity = rigidBody.velocity +
                                  transform.GetForwardDirection() * 2000.f,
                      .isKinematic = false,
                      .octree      = octree });
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