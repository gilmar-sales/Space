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

    bool Intersect(const Particle& other) const
    {
        return glm::distance(transform.position, other.transform.position) <=
               sphereCollider.radius + other.sphereCollider.radius;
    }
};

class Octree
{
  public:
    Octree(glm::vec3 position, float halfRange, size_t capacity);
    ~Octree() = default;

    bool Contains(const Particle& particle) const;
    bool Insert(Particle particle);
    void Subdivide();

    void Query(Particle& particle, std::vector<Particle*>& found);
    bool Intersect(const Particle& particle) const;

    void Query(const Frustum& frustum, std::vector<Particle*>& found);

    void Draw(std::shared_ptr<fra::Renderer> renderer,
              std::shared_ptr<fra::MeshPool>
                                                meshPool,
              const std::vector<std::uint32_t>& meshIds);
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