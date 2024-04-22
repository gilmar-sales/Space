#include "CollisionSystem.hpp"

#include <Asset/MeshPool.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Events/CollisionEvent.hpp"

#include "Components/RigidBodyComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"
#include <Events/OctreeFinishedEvent.hpp>

void CollisionSystem::Start()
{
    // mSphereModel = mMeshPool->CreateMeshFromFile("C:/Models/debug_sphere.obj");
    // mCubeModel   = mMeshPool->CreateMeshFromFile("C:/Models/debug_cube.obj");

    mManager->AddEventListener<OctreeFinishedEvent>([](OctreeFinishedEvent octreeFinishedEvent) {

    });
}

void CollisionSystem::Update(float deltaTime)
{
    auto octree = mOctreeSystem->GetOctree();
    mManager->ForEachAsync<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
        [octree = octree, manager = mManager,
         deltaTime = deltaTime](fr::Entity entity, TransformComponent& transform,
                                SphereColliderComponent& sphereCollider, RigidBodyComponent& rigidBody) {
            auto collisions = std::vector<Particle*>(0);

            auto particle = Particle { .entity = entity, .transform = transform, .sphereCollider = sphereCollider };

            octree->Query(particle, collisions);

            for (const auto collision : collisions)
            {
                manager->SendEvent<CollisionEvent>(
                    CollisionEvent { .target = entity, .collisor = collision->entity, .deltaTime = deltaTime });
            }
        });
}
