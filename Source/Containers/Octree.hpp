#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>
#include <glm/glm.hpp>

#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "ArenaAllocator.hpp"
#include "Frustum.hpp"
#include "LockFreeArray.hpp"

struct Particle
{
    fr::Entity               entity;
    TransformComponent*      transform;
    SphereColliderComponent* sphereCollider;

    [[nodiscard]] bool Intersect(const Particle& other) const
    {
        return glm::distance(transform->position, other.transform->position) <=
               sphereCollider->radius + other.sphereCollider->radius;
    }
};

class Octree
{
    enum class State : uint32_t
    {
        Leaf,
        Subdividing,
        Branch
    };

  public:
    Octree(glm::vec3       position,
           float           halfRange,
           size_t          capacity,
           ArenaAllocator* allocator = new ArenaAllocator(),
           Octree*         root      = nullptr);

    ~Octree() {}

    bool    Contains(const Particle& particle) const;
    Octree* Insert(const Particle& particle);
    void    Remove(fr::Entity entity);
    void    TrySubdivide();

    void Query(Particle& particle, std::vector<Particle>& found);
    bool Intersect(const Particle& particle) const;

    void Query(const Frustum& frustum, std::vector<Particle>& found);
    bool IsInsideFrustum(const Frustum& frustum) const;

    void Draw(const Ref<fra::Renderer>&         renderer,
              const Ref<fra::MeshPool>&         meshPool,
              const std::vector<std::uint32_t>& meshIds) const;
    void PushInstanceData(std::vector<glm::mat4>& instanceData) const;

  private:
    friend struct Particle;

    Octree*         mRoot;
    ArenaAllocator* mAllocator;

    glm::vec3 mPosition;
    size_t    mCapacity;
    float     mHalfRange;

    LockFreeArray<Particle> mElements;
    std::atomic<State>      mState;

    Octree* mNearTopLeft;
    Octree* mNearTopRight;
    Octree* mNearBotLeft;
    Octree* mNearBotRight;

    Octree* mFarTopLeft;
    Octree* mFarTopRight;
    Octree* mFarBotLeft;
    Octree* mFarBotRight;
};