#include "Octree.hpp"

#include <glm/ext/matrix_transform.hpp>

Octree::Octree(glm::vec3 position, float halfRange, size_t capacity)
{
    mPosition  = position;
    mCapacity  = capacity;
    mHalfRange = halfRange;
    mElements.reserve(capacity);
}

Octree::Octree(const Octree&)
{
}

bool Octree::Contains(const Particle& particle)
{
    return (particle.transform.position.x >= mPosition.x - mHalfRange &&
            particle.transform.position.x <= mPosition.x + mHalfRange &&
            particle.transform.position.y >= mPosition.y - mHalfRange &&
            particle.transform.position.y <= mPosition.y + mHalfRange &&
            particle.transform.position.z >= mPosition.z - mHalfRange &&
            particle.transform.position.z <= mPosition.z + mHalfRange);
}

bool Octree::Insert(Particle particle)
{
    if (!Contains(particle))
    {
        return false;
    }

    if (mElements.size() < mCapacity)
    {
        mElements.push_back(particle);
        return true;
    }
    else if (mFarTopLeft == nullptr)
    {
        Subdivide();
    }

    return (mNearTopLeft->Insert(particle) || mNearTopRight->Insert(particle) || mNearBotLeft->Insert(particle) ||
            mNearBotRight->Insert(particle) || mFarTopLeft->Insert(particle) || mFarTopRight->Insert(particle) ||
            mFarBotLeft->Insert(particle) || mFarBotRight->Insert(particle));
}

void Octree::Subdivide()
{
    float halfRange = mHalfRange / 2;

    glm::vec3 nearTopLeftPos  = { mPosition.x - halfRange, mPosition.y - halfRange, mPosition.z - halfRange };
    glm::vec3 nearTopRightPos = { mPosition.x + halfRange, mPosition.y - halfRange, mPosition.z - halfRange };
    glm::vec3 nearBotLeftPos  = { mPosition.x - halfRange, mPosition.y + halfRange, mPosition.z - halfRange };
    glm::vec3 nearBotRightPos = { mPosition.x + halfRange, mPosition.y + halfRange, mPosition.z - halfRange };

    mNearTopLeft  = std::make_unique<Octree>(nearTopLeftPos, halfRange, mCapacity);
    mNearTopRight = std::make_unique<Octree>(nearTopRightPos, halfRange, mCapacity);
    mNearBotLeft  = std::make_unique<Octree>(nearBotLeftPos, halfRange, mCapacity);
    mNearBotRight = std::make_unique<Octree>(nearBotRightPos, halfRange, mCapacity);

    glm::vec3 farTopLeftPos  = { mPosition.x - halfRange, mPosition.y - halfRange, mPosition.z + halfRange };
    glm::vec3 farTopRightPos = { mPosition.x + halfRange, mPosition.y - halfRange, mPosition.z + halfRange };
    glm::vec3 farBotLeftPos  = { mPosition.x - halfRange, mPosition.y + halfRange, mPosition.z + halfRange };
    glm::vec3 farBotRightPos = { mPosition.x + halfRange, mPosition.y + halfRange, mPosition.z + halfRange };

    mFarTopLeft  = std::make_unique<Octree>(farTopLeftPos, halfRange, mCapacity);
    mFarTopRight = std::make_unique<Octree>(farTopRightPos, halfRange, mCapacity);
    mFarBotLeft  = std::make_unique<Octree>(farBotLeftPos, halfRange, mCapacity);
    mFarBotRight = std::make_unique<Octree>(farBotRightPos, halfRange, mCapacity);
}

void Octree::Query(Particle& particle, std::vector<Particle*>& found)
{
    if (!Intersect(particle))
    {
        return;
    }

    for (auto& other : mElements)
    {
        if (particle.entity == other.entity)
            continue;

        if (particle.Intersect(other))
        {
            found.push_back(&other);
        }
    }

    if (mNearTopLeft)
    {
        mNearTopLeft->Query(particle, found);
        mNearTopRight->Query(particle, found);
        mNearBotLeft->Query(particle, found);
        mNearBotRight->Query(particle, found);
        mFarTopLeft->Query(particle, found);
        mFarTopRight->Query(particle, found);
        mFarBotLeft->Query(particle, found);
        mFarBotRight->Query(particle, found);
    }
}

bool Octree::Intersect(const Particle& particle)
{
    return (particle.transform.position.x >= mPosition.x - (mHalfRange + particle.sphereCollider.radius) &&
            particle.transform.position.x <= mPosition.x + (mHalfRange + particle.sphereCollider.radius) &&
            particle.transform.position.y >= mPosition.y - (mHalfRange + particle.sphereCollider.radius) &&
            particle.transform.position.y <= mPosition.y + (mHalfRange + particle.sphereCollider.radius) &&
            particle.transform.position.z >= mPosition.z - (mHalfRange + particle.sphereCollider.radius) &&
            particle.transform.position.z <= mPosition.z + (mHalfRange + particle.sphereCollider.radius));
}

void Octree::Query(Frustum& frustum, std::vector<Particle*>& found)
{
    if (!Intersect(frustum))
    {
        return;
    }

    for (auto& other : mElements)
    {
        if (other.isOnFrustum(frustum))
        {
            found.push_back(&other);
        }
    }

    if (mNearTopLeft)
    {
        mNearTopLeft->Query(frustum, found);
        mNearTopRight->Query(frustum, found);
        mNearBotLeft->Query(frustum, found);
        mNearBotRight->Query(frustum, found);
        mFarTopLeft->Query(frustum, found);
        mFarTopRight->Query(frustum, found);
        mFarBotLeft->Query(frustum, found);
        mFarBotRight->Query(frustum, found);
    }
}

// see https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
bool Octree::isOnOrForwardPlane(const Plane& plane) const
{
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    const float r = mHalfRange * std::abs(plane.normal.x) + mHalfRange * std::abs(plane.normal.y) +
                    mHalfRange * std::abs(plane.normal.z);

    return -r <= plane.getSignedDistanceToPlane(mPosition);
}

bool Octree::Intersect(const Frustum& camFrustum)
{
    return (isOnOrForwardPlane(camFrustum.leftFace) && isOnOrForwardPlane(camFrustum.rightFace) &&
            isOnOrForwardPlane(camFrustum.topFace) && isOnOrForwardPlane(camFrustum.bottomFace) &&
            isOnOrForwardPlane(camFrustum.nearFace) && isOnOrForwardPlane(camFrustum.farFace));
}

void Octree::PushInstanceData(std::vector<glm::mat4>& instanceData)
{
    auto modelMatrix = glm::mat4(1);

    modelMatrix = glm::translate(modelMatrix, mPosition);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(mHalfRange));

    instanceData.push_back(modelMatrix);

    if (mNearTopLeft != nullptr)
    {
        mNearTopLeft->PushInstanceData(instanceData);
        mNearTopRight->PushInstanceData(instanceData);
        mNearBotLeft->PushInstanceData(instanceData);
        mNearBotRight->PushInstanceData(instanceData);
        mFarTopLeft->PushInstanceData(instanceData);
        mFarTopRight->PushInstanceData(instanceData);
        mFarBotLeft->PushInstanceData(instanceData);
        mFarBotRight->PushInstanceData(instanceData);
    }
}

void Octree::Draw(std::shared_ptr<fra::Renderer> renderer,
                  std::shared_ptr<fra::MeshPool>
                                              meshPool,
                  std::vector<std::uint32_t>& meshIds)
{
    static std::shared_ptr<fra::Buffer> instanceBuffer = nullptr;
    auto                                instanceData   = std::vector<glm::mat4>();

    PushInstanceData(instanceData);

    instanceBuffer =
        renderer->GetBufferBuilder()
            .SetData(instanceData.data())
            .SetSize(sizeof(glm::mat4) * instanceData.size())
            .SetUsage(fra::BufferUsage::Instance)
            .Build();

    renderer->BindBuffer(instanceBuffer);

    for (auto meshId : meshIds)
    {
        meshPool->DrawInstanced(meshId, instanceData.size());
    }
}
