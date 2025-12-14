#pragma once

#include <Freyr/Freyr.hpp>

#include "Components/RigidBodyComponent.hpp"

#include <Containers/Octree.hpp>

class OctreeSystem final : public fr::System
{
  public:
    explicit OctreeSystem(const Ref<fr::Scene>& scene) :
        System(scene), mAllocator(new ArenaAllocator()), mKinematicAllocator(new ArenaAllocator()), mOctree(nullptr),
        mKinematicOctree(nullptr), mEntries()
    {
        BuildOctree();
    }

    ~OctreeSystem() override = default;

    void PreFixedUpdate(float deltaTime) override;

    void Query(Particle& particle, std::vector<Particle>& found);

    void Query(const Frustum& frustum, std::vector<Particle>& found)
    {
        if (mKinematicOctree != nullptr)
            mKinematicOctree->Query(frustum, found);

        if (mOctree != nullptr)
            mOctree->Query(frustum, found);
    }

    void Insert(const Particle& particle)
    {
        if (mOctree != nullptr)
            mOctree->Insert(particle);
    }

    void InsertKinematic(const Particle& particle)
    {
        if (mKinematicOctree != nullptr)
        {
            Entry entry = { .entity = particle.entity, .node = mKinematicOctree->Insert(particle) };
            mEntries.insert(entry);
        }
    }

    void Remove(fr::Entity entity)
    {
        if (!mEntries.contains(entity))
            return;

        const auto entry = mEntries[entity];

        entry.node->Remove(entity);

        mEntries.remove(entry);
    }

  private:
    void BuildOctree();

    struct Entry
    {
        fr::Entity entity;
        Octree*    node;

        operator size_t() { return entity; }
    };

    fr::SparseSet<Entry> mEntries;

    ArenaAllocator* mKinematicAllocator;
    Octree*         mKinematicOctree;

    ArenaAllocator* mAllocator;
    Octree*         mOctree;
};
