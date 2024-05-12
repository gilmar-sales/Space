#pragma once

#include <Core/Renderer.hpp>
#include <Freyr/Freyr.hpp>
#include <glm/glm.hpp>

#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Frustum.hpp"

struct Particle
{
    fr::Entity               entity;
    TransformComponent&      transform;
    SphereColliderComponent& sphereCollider;

    bool Intersect(const Particle& other)
    {
        return glm::distance(transform.position, other.transform.position) <=
               sphereCollider.radius + other.sphereCollider.radius;
    }

    bool isOnOrForwardPlane(const Plane& plane) const
    {
        return plane.getSignedDistanceToPlane(transform.position) >
               -sphereCollider.radius;
    }

    bool isOnFrustum(const Frustum& camFrustum) const
    {
        return (isOnOrForwardPlane(camFrustum.leftFace) &&
                isOnOrForwardPlane(camFrustum.rightFace) &&
                isOnOrForwardPlane(camFrustum.farFace) &&
                isOnOrForwardPlane(camFrustum.nearFace) &&
                isOnOrForwardPlane(camFrustum.topFace) &&
                isOnOrForwardPlane(camFrustum.bottomFace));
    }
};

class Octree
{
  public:
    Octree(glm::vec3 position, float halfRange, size_t capacity);
    ~Octree() = default;
    Octree(const Octree&);

    bool Contains(const Particle& particle);
    bool Insert(Particle particle);
    void Subdivide();

    void Query(Particle& particle, std::vector<Particle*>& found);
    bool Intersect(const Particle& particle);

    void Query(Frustum& frustum, std::vector<Particle*>& found);
    bool isOnOrForwardPlane(const Plane& plane) const;
    bool Intersect(const Frustum& particle);

    void Draw(std::shared_ptr<fra::Renderer> renderer,
              std::shared_ptr<fra::MeshPool>
                                          meshPool,
              std::vector<std::uint32_t>& meshIds);
    void PushInstanceData(std::vector<glm::mat4>& instanceData);

  private:
    std::mutex mMutex;

    glm::vec3             mPosition;
    size_t                mCapacity;
    float                 mHalfRange;
    std::vector<Particle> mElements;

    std::unique_ptr<Octree> mNearTopLeft;
    std::unique_ptr<Octree> mNearTopRight;
    std::unique_ptr<Octree> mNearBotLeft;
    std::unique_ptr<Octree> mNearBotRight;

    std::unique_ptr<Octree> mFarTopLeft;
    std::unique_ptr<Octree> mFarTopRight;
    std::unique_ptr<Octree> mFarBotLeft;
    std::unique_ptr<Octree> mFarBotRight;
};