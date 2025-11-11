#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>
#include <glm/glm.hpp>

#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "Frustum.hpp"
#include "LockFreeArray.hpp"
class Octree;

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
    Octree(glm::vec3                     position,
           float                         halfRange,
           size_t                        capacity,
           const std::allocator<Octree>& allocator = std::allocator<Octree>());
    ~Octree() = default;

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

    std::mutex             mMutex;
    std::allocator<Octree> mAllocator;

    glm::vec3               mPosition;
    size_t                  mCapacity;
    float                   mHalfRange;
    LockFreeArray<Particle> mElements;
    std::atomic<State>      mState;
    Ref<Octree>             mNearTopLeft;
    Ref<Octree>             mNearTopRight;
    Ref<Octree>             mNearBotLeft;
    Ref<Octree>             mNearBotRight;

    Ref<Octree> mFarTopLeft;
    Ref<Octree> mFarTopRight;
    Ref<Octree> mFarBotLeft;
    Ref<Octree> mFarBotRight;
};