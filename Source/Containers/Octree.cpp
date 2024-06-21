#include "Octree.hpp"

#include <glm/ext/matrix_transform.hpp>

Octree::Octree(glm::vec3 position, float halfRange, size_t capacity) :
    mPosition(position), mCapacity(capacity), mHalfRange(halfRange)
{
    mElements.reserve(capacity);
}

bool Octree::Contains(const Particle& particle) const
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

    {
        auto lock = std::lock_guard(mMutex);
        if (mElements.size() < mCapacity)
        {
            mElements.push_back(particle);
            return true;
        }
        else if (mFarTopLeft == nullptr)
        {
            Subdivide();
        }
    }

    return (mNearTopLeft->Insert(particle) || mNearTopRight->Insert(particle) ||
            mNearBotLeft->Insert(particle) || mNearBotRight->Insert(particle) ||
            mFarTopLeft->Insert(particle) || mFarTopRight->Insert(particle) ||
            mFarBotLeft->Insert(particle) || mFarBotRight->Insert(particle));
}

void Octree::Subdivide()
{
    float halfRange = mHalfRange / 2;

    glm::vec3 nearTopLeftPos  = { mPosition.x - halfRange,
                                  mPosition.y - halfRange,
                                  mPosition.z - halfRange };
    glm::vec3 nearTopRightPos = { mPosition.x + halfRange,
                                  mPosition.y - halfRange,
                                  mPosition.z - halfRange };
    glm::vec3 nearBotLeftPos  = { mPosition.x - halfRange,
                                  mPosition.y + halfRange,
                                  mPosition.z - halfRange };
    glm::vec3 nearBotRightPos = { mPosition.x + halfRange,
                                  mPosition.y + halfRange,
                                  mPosition.z - halfRange };

    mNearTopLeft =
        std::make_unique<Octree>(nearTopLeftPos, halfRange, mCapacity);
    mNearTopRight =
        std::make_unique<Octree>(nearTopRightPos, halfRange, mCapacity);
    mNearBotLeft =
        std::make_unique<Octree>(nearBotLeftPos, halfRange, mCapacity);
    mNearBotRight =
        std::make_unique<Octree>(nearBotRightPos, halfRange, mCapacity);

    glm::vec3 farTopLeftPos  = { mPosition.x - halfRange,
                                 mPosition.y - halfRange,
                                 mPosition.z + halfRange };
    glm::vec3 farTopRightPos = { mPosition.x + halfRange,
                                 mPosition.y - halfRange,
                                 mPosition.z + halfRange };
    glm::vec3 farBotLeftPos  = { mPosition.x - halfRange,
                                 mPosition.y + halfRange,
                                 mPosition.z + halfRange };
    glm::vec3 farBotRightPos = { mPosition.x + halfRange,
                                 mPosition.y + halfRange,
                                 mPosition.z + halfRange };

    mFarTopLeft = std::make_unique<Octree>(farTopLeftPos, halfRange, mCapacity);
    mFarTopRight =
        std::make_unique<Octree>(farTopRightPos, halfRange, mCapacity);
    mFarBotLeft = std::make_unique<Octree>(farBotLeftPos, halfRange, mCapacity);
    mFarBotRight =
        std::make_unique<Octree>(farBotRightPos, halfRange, mCapacity);
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
        if (mNearTopLeft->mElements.size())
            mNearTopLeft->Query(particle, found);

        if (mNearTopRight->mElements.size())
            mNearTopRight->Query(particle, found);

        if (mNearBotLeft->mElements.size())
            mNearBotLeft->Query(particle, found);

        if (mNearBotRight->mElements.size())
            mNearBotRight->Query(particle, found);

        if (mFarTopLeft->mElements.size())
            mFarTopLeft->Query(particle, found);

        if (mFarTopRight->mElements.size())
            mFarTopRight->Query(particle, found);

        if (mFarBotLeft->mElements.size())
            mFarBotLeft->Query(particle, found);

        if (mFarBotRight->mElements.size())
            mFarBotRight->Query(particle, found);
    }
}

bool Octree::Intersect(const Particle& particle) const
{
    return (particle.transform.position.x >=
                mPosition.x - (mHalfRange + particle.sphereCollider.radius) &&
            particle.transform.position.x <=
                mPosition.x + (mHalfRange + particle.sphereCollider.radius) &&
            particle.transform.position.y >=
                mPosition.y - (mHalfRange + particle.sphereCollider.radius) &&
            particle.transform.position.y <=
                mPosition.y + (mHalfRange + particle.sphereCollider.radius) &&
            particle.transform.position.z >=
                mPosition.z - (mHalfRange + particle.sphereCollider.radius) &&
            particle.transform.position.z <=
                mPosition.z + (mHalfRange + particle.sphereCollider.radius));
}

bool Octree::IsInsideFrustum(const Frustum& frustum) const
{
    glm::vec3 halfSize = glm::vec3(mHalfRange);

    for (int i = 0; i < 6; ++i)
    {
        glm::vec3 p = glm::vec3(
            frustum.planes[i].normal.x > 0 ? mPosition.x + halfSize.x
                                           : mPosition.x - halfSize.x,
            frustum.planes[i].normal.y > 0 ? mPosition.y + halfSize.y
                                           : mPosition.y - halfSize.y,
            frustum.planes[i].normal.z > 0 ? mPosition.z + halfSize.z
                                           : mPosition.z - halfSize.z);

        if (glm::dot(frustum.planes[i].normal, p) + frustum.planes[i].distance <
            0)
        {
            return false;
        }
    };

    return true;
}

void Octree::Query(const Frustum& frustum, std::vector<Particle*>& found)
{
    if (!frustum.SphereIntersect(mPosition, mHalfRange * 1.735f))
        return;

    for (auto& other : mElements)
    {
        if (frustum.SphereIntersect(other.transform.position,
                                    other.sphereCollider.radius))
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
                  const std::vector<std::uint32_t>& meshIds)
{
    static std::shared_ptr<fra::Buffer> instanceBuffer = nullptr;
    auto                                instanceData = std::vector<glm::mat4>();

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
