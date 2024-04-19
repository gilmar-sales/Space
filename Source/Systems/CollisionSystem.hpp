#pragma once

#include <Core/Renderer.hpp>
#include <Freyr.hpp>
#include <Containers/Octree.hpp>

class CollisionSystem : public fr::System
{
    public:
        void Start() override;
        void Update(float deltaTime) override;
    private:
        friend class SpaceApp;

        std::vector<std::uint32_t> mSphereModel;
        std::vector<std::uint32_t> mCubeModel;
        std::shared_ptr<fra::Renderer> mRenderer;
        std::shared_ptr<Octree> mOctree;
};