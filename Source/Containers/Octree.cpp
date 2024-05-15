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

bool Octree::Intersect(const Frustum& frustum) const
{
    glm::vec3 nearCenter = frustum.apex;
    glm::vec3 farCenter  = {
        frustum.apex.x + frustum.direction.x * frustum.farPlane,
        frustum.apex.y + frustum.direction.y * frustum.farPlane,
        frustum.apex.z + frustum.direction.z * frustum.farPlane
    };

    double tanHalfFOV = tan(frustum.fovAngle / 2);
    double nearHeight = 2 * tanHalfFOV * frustum.nearPlane;
    double farHeight  = 2 * tanHalfFOV * frustum.farPlane;

    glm::vec3 nearTop = { nearCenter.x + frustum.direction.x * nearHeight / 2,
                          nearCenter.y + frustum.direction.y * nearHeight / 2,
                          nearCenter.z + frustum.direction.z * nearHeight / 2 };

    glm::vec3 farTop = { farCenter.x + frustum.direction.x * farHeight / 2,
                         farCenter.y + frustum.direction.y * farHeight / 2,
                         farCenter.z + frustum.direction.z * farHeight / 2 };

    if (double frontZ = mPosition.z + mHalfRange; frontZ > frustum.nearPlane)
    {
        double frontSize =
            (frustum.nearPlane * mHalfRange * 2) / (frontZ - frustum.apex.z);
        if (frontSize <= mHalfRange * 2)
        {
            return true;
        }
    }

    if (double backZ = mPosition.z - mHalfRange; backZ < frustum.farPlane)
    {
        double backSize =
            (frustum.farPlane * mHalfRange * 2) / (backZ - frustum.apex.z);
        if (backSize <= mHalfRange * 2)
        {
            return true;
        }
    }

    if (double leftX = mPosition.x - mHalfRange; leftX > frustum.apex.x)
    {
        double leftSize = (leftX - frustum.apex.x) * tanHalfFOV * 2;
        if (leftSize <= mHalfRange * 2)
        {
            return true;
        }
    }

    if (double rightX = mPosition.x + mHalfRange; rightX < frustum.apex.x)
    {
        double rightSize = (frustum.apex.x - rightX) * tanHalfFOV * 2;
        if (rightSize <= mHalfRange * 2)
        {
            return true;
        }
    }

    if (double topY = mPosition.y + mHalfRange;
        topY < nearTop.y && topY < farTop.y)
    {
        double topSize = (nearTop.y - topY) * tanHalfFOV * 2;
        if (topSize <= mHalfRange * 2)
        {
            return true;
        }
    }

    if (double bottomY = mPosition.y - mHalfRange;
        bottomY > nearCenter.y && bottomY > farCenter.y)
    {
        double bottomSize = (bottomY - nearCenter.y) * tanHalfFOV * 2;
        if (bottomSize <= mHalfRange * 2)
        {
            return true;
        }
    }

    return false;
}

void Octree::Query(const Frustum& frustum, std::vector<Particle*>& found)
{
    if (!Intersect(frustum))
    {
        return;
    }

    for (auto& other : mElements)
    {
        if (other.Intersect(frustum))
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

bool Particle::Intersect(const Frustum& frustum) const
{
    glm::vec3 nearCenter = frustum.apex;
    glm::vec3 farCenter  = {
        frustum.apex.x + frustum.direction.x * frustum.farPlane,
        frustum.apex.y + frustum.direction.y * frustum.farPlane,
        frustum.apex.z + frustum.direction.z * frustum.farPlane
    };

    double tanHalfFOV = tan(frustum.fovAngle / 2);
    double nearHeight = 2 * tanHalfFOV * frustum.nearPlane;
    double farHeight  = 2 * tanHalfFOV * frustum.farPlane;

    glm::vec3 nearTop = { nearCenter.x + frustum.direction.x * nearHeight / 2,
                          nearCenter.y + frustum.direction.y * nearHeight / 2,
                          nearCenter.z + frustum.direction.z * nearHeight / 2 };

    glm::vec3 farTop = { farCenter.x + frustum.direction.x * farHeight / 2,
                         farCenter.y + frustum.direction.y * farHeight / 2,
                         farCenter.z + frustum.direction.z * farHeight / 2 };

    glm::vec3 apexToCenter = { transform.position.x - frustum.apex.x,
                               transform.position.y - frustum.apex.y,
                               transform.position.z - frustum.apex.z };

    double projection = (apexToCenter.x * frustum.direction.x) +
                        (apexToCenter.y * frustum.direction.y) +
                        (apexToCenter.z * frustum.direction.z);

    if (projection + sphereCollider.radius < 0)
    {
        return false;
    }

    if (double distanceToNearPlane = projection;
        distanceToNearPlane < frustum.nearPlane + sphereCollider.radius)
    {
        return true;
    }

    if (double distanceToFarPlane = projection;
        distanceToFarPlane < frustum.farPlane + sphereCollider.radius)
    {
        return true;
    }

    double nearRadius = frustum.nearPlane * tanHalfFOV;

    if (double distanceToNearLeftPlane = projection - nearRadius;
        distanceToNearLeftPlane > 0 &&
        distanceToNearLeftPlane < frustum.nearPlane)
    {
        return true;
    }

    if (double distanceToNearRightPlane = projection + nearRadius;
        distanceToNearRightPlane > 0 &&
        distanceToNearRightPlane < frustum.nearPlane)
    {
        return true;
    }

    double farRadius = frustum.farPlane * tanHalfFOV;

    if (double distanceToFarLeftPlane = projection - farRadius;
        distanceToFarLeftPlane > 0 && distanceToFarLeftPlane < frustum.farPlane)
    {
        return true;
    }

    if (double distanceToFarRightPlane = projection + farRadius;
        distanceToFarRightPlane > 0 &&
        distanceToFarRightPlane < frustum.farPlane)
    {
        return true;
    }

    return false;
}
