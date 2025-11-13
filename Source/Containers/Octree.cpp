#include "Octree.hpp"

#include <glm/ext/matrix_transform.hpp>

void Octree::Remove(fr::Entity entity)
{
    for (auto i = 0; i < mElements.size(); i++)
    {
        if (auto element = mElements.get(i); element.has_value() && element->entity == entity)
            mElements.remove(i);
    }
}

Octree::Octree(const glm::vec3 position, const float halfRange, const size_t capacity,
               std::allocator<Octree>* allocator, Octree* root) :
    mAllocator(allocator), mPosition(position), mCapacity(capacity), mHalfRange(halfRange), mRoot(root)
{
    if (mRoot == nullptr)
        mRoot = this;
}

bool Octree::Contains(const Particle& particle) const
{
    return (particle.transform->position.x >= mPosition.x - mHalfRange &&
            particle.transform->position.x <= mPosition.x + mHalfRange &&
            particle.transform->position.y >= mPosition.y - mHalfRange &&
            particle.transform->position.y <= mPosition.y + mHalfRange &&
            particle.transform->position.z >= mPosition.z - mHalfRange &&
            particle.transform->position.z <= mPosition.z + mHalfRange);
}

Octree* Octree::Insert(const Particle& particle)
{
    if (!Contains(particle))
    {
        return nullptr;
    }

    while (true)
    {
        State current_state = mState.load(std::memory_order_acquire);

        if (current_state == State::Leaf)
        {
            // Try to add to leaf node
            if (mElements.push(particle))
            {
                return this;
            }

            TrySubdivide();
        }
        else if (current_state == State::Subdividing)
        {
            // Another thread is subdividing, spin wait
            std::this_thread::yield();
            continue;
        }
        else // State::Branch
        {
            auto ptr = mNearTopLeft->Insert(particle);

            if (ptr)
                return ptr;

            ptr = mNearTopRight->Insert(particle);

            if (ptr)
                return ptr;

            ptr = mNearBotLeft->Insert(particle);

            if (ptr)
                return ptr;

            ptr = mNearBotRight->Insert(particle);

            if (ptr)
                return ptr;

            ptr = mFarTopLeft->Insert(particle);

            if (ptr)
                return ptr;

            ptr = mFarTopRight->Insert(particle);

            if (ptr)
                return ptr;

            ptr = mFarBotLeft->Insert(particle);

            if (ptr)
                return ptr;

            ptr = mFarBotRight->Insert(particle);

            if (ptr)
                return ptr;

            return nullptr;
        }
    }
}

void Octree::TrySubdivide()
{
    State expected = State::Leaf;

    if (!mState.compare_exchange_strong(
            expected, State::Subdividing, std::memory_order_acquire, std::memory_order_release))
    {
        return;
    }

    float halfRange = mHalfRange / 2;

    glm::vec3 nearTopLeftPos  = { mPosition.x - halfRange, mPosition.y - halfRange, mPosition.z - halfRange };
    glm::vec3 nearTopRightPos = { mPosition.x + halfRange, mPosition.y - halfRange, mPosition.z - halfRange };
    glm::vec3 nearBotLeftPos  = { mPosition.x - halfRange, mPosition.y + halfRange, mPosition.z - halfRange };
    glm::vec3 nearBotRightPos = { mPosition.x + halfRange, mPosition.y + halfRange, mPosition.z - halfRange };

    mNearTopLeft = std::allocate_shared<Octree>(*mAllocator, nearTopLeftPos, halfRange, mCapacity, mAllocator, mRoot);

    mNearTopRight = std::allocate_shared<Octree>(*mAllocator, nearTopRightPos, halfRange, mCapacity, mAllocator, mRoot);

    mNearBotLeft = std::allocate_shared<Octree>(*mAllocator, nearBotLeftPos, halfRange, mCapacity, mAllocator, mRoot);

    mNearBotRight = std::allocate_shared<Octree>(*mAllocator, nearBotRightPos, halfRange, mCapacity, mAllocator, mRoot);

    glm::vec3 farTopLeftPos  = { mPosition.x - halfRange, mPosition.y - halfRange, mPosition.z + halfRange };
    glm::vec3 farTopRightPos = { mPosition.x + halfRange, mPosition.y - halfRange, mPosition.z + halfRange };
    glm::vec3 farBotLeftPos  = { mPosition.x - halfRange, mPosition.y + halfRange, mPosition.z + halfRange };
    glm::vec3 farBotRightPos = { mPosition.x + halfRange, mPosition.y + halfRange, mPosition.z + halfRange };

    mFarTopLeft  = std::allocate_shared<Octree>(*mAllocator, farTopLeftPos, halfRange, mCapacity, mAllocator, mRoot);
    mFarTopRight = std::allocate_shared<Octree>(*mAllocator, farTopRightPos, halfRange, mCapacity, mAllocator, mRoot);
    mFarBotLeft  = std::allocate_shared<Octree>(*mAllocator, farBotLeftPos, halfRange, mCapacity, mAllocator, mRoot);
    mFarBotRight = std::allocate_shared<Octree>(*mAllocator, farBotRightPos, halfRange, mCapacity, mAllocator, mRoot);

    mState.store(State::Branch, std::memory_order_release);
}

void Octree::Query(Particle& particle, std::vector<Particle>& found)
{
    if (!Intersect(particle))
    {
        return;
    }

    for (auto i = 0; i < mElements.size(); i++)
    {
        auto other = mElements.get(i);

        if (!other.has_value() || particle.entity == other->entity)
            continue;

        if (particle.Intersect(*other))
        {
            found.push_back(*other);
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

bool Octree::Intersect(const Particle& particle) const
{
    return (particle.transform->position.x >= mPosition.x - (mHalfRange + particle.sphereCollider->radius) &&
            particle.transform->position.x <= mPosition.x + (mHalfRange + particle.sphereCollider->radius) &&
            particle.transform->position.y >= mPosition.y - (mHalfRange + particle.sphereCollider->radius) &&
            particle.transform->position.y <= mPosition.y + (mHalfRange + particle.sphereCollider->radius) &&
            particle.transform->position.z >= mPosition.z - (mHalfRange + particle.sphereCollider->radius) &&
            particle.transform->position.z <= mPosition.z + (mHalfRange + particle.sphereCollider->radius));
}

bool Octree::IsInsideFrustum(const Frustum& frustum) const
{
    const auto halfSize = glm::vec3(mHalfRange);

    for (const auto& [normal, distance] : frustum.planes)
    {
        auto p = glm::vec3(normal.x > 0 ? mPosition.x + halfSize.x : mPosition.x - halfSize.x,
                           normal.y > 0 ? mPosition.y + halfSize.y : mPosition.y - halfSize.y,
                           normal.z > 0 ? mPosition.z + halfSize.z : mPosition.z - halfSize.z);

        if (glm::dot(normal, p) + distance < 0)
        {
            return false;
        }
    };

    return true;
}

void Octree::Query(const Frustum& frustum, std::vector<Particle>& found)
{
    if (!frustum.SphereIntersect(mPosition, mHalfRange * 1.735f))
        return;

    for (auto i = 0; i < mElements.size(); i++)
    {
        auto other = mElements.get(i);
        if (other.has_value() && frustum.SphereIntersect(other->transform->position, other->sphereCollider->radius))
        {
            found.emplace_back(*other);
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

void Octree::PushInstanceData(std::vector<glm::mat4>& instanceData) const
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

void Octree::Draw(const Ref<fra::Renderer>&         renderer,
                  const Ref<fra::MeshPool>&         meshPool,
                  const std::vector<std::uint32_t>& meshIds) const
{
    static Ref<fra::Buffer> instanceBuffer = nullptr;
    auto                    instanceData   = std::vector<glm::mat4>();

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
