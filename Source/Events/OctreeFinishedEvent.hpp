#pragma once

#include <Containers/Octree.hpp>
#include <Freyr/Freyr.hpp>

struct OctreeFinishedEvent : fr::Event
{
    std::shared_ptr<Octree> octree;
};