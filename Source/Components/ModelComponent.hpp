#pragma once

#include <Freyr/Freyr.hpp>

struct ModelComponent : fr::Component
{
    std::vector<std::uint32_t>* meshes;
    std::uint32_t               texture;
};