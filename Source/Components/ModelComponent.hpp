#pragma once

#include <Asset/MeshPool.hpp>
#include <Freyr.hpp>

struct ModelComponent : fr::Component
{
    std::vector<std::uint32_t>* meshes;
};