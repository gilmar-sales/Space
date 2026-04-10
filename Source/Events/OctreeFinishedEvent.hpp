#pragma once

#include <Containers/Octree.hpp>
#include <Freyr/Freyr.hpp>

struct OctreeFinishedEvent : fr::Event
{
    Ref<Octree> octree;
};