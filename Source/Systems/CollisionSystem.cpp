#include "CollisionSystem.hpp"

#include <Asset/MeshPool.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Events/CollisionEvent.hpp"

#include "Components/RigidBodyComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"

extern std::shared_ptr<fra::MeshPool> gMeshPool;

void CollisionSystem::Start()
{
    mSphereModel = gMeshPool->CreateMeshFromFile("C:/Models/debug_sphere.obj");
    mCubeModel   = gMeshPool->CreateMeshFromFile("C:/Models/debug_cube.obj");
}

void CollisionSystem::Update(float deltaTime)
{
    mOctree = std::make_shared<Octree>(glm::vec3(0), 1000.0f, 4);

    mManager->ForEach<TransformComponent, SphereColliderComponent>([octree = mOctree](fr::Entity entity, TransformComponent& transform, SphereColliderComponent& sphereCollider) {
        octree->Insert(Particle {
            .entity         = entity,
            .transform      = transform,
            .sphereCollider = sphereCollider });
    });

    mManager->ForEachAsync<TransformComponent, SphereColliderComponent, RigidBodyComponent>([octree = mOctree, manager = mManager, deltaTime = deltaTime](fr::Entity entity, TransformComponent& transform, SphereColliderComponent& sphereCollider, RigidBodyComponent& rigidBody) {
        auto collisions = std::vector<Particle*>(0);

        auto particle = Particle {
            .entity         = entity,
            .transform      = transform,
            .sphereCollider = sphereCollider
        };

        octree->Query(particle, collisions);

        for (const auto collision : collisions)
        {
            manager->SendEvent<CollisionEvent>(CollisionEvent { .target = entity, .collisor = collision->entity, .deltaTime = deltaTime });
        }
    });
}
