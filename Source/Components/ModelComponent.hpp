#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

#include <vector>

struct ModelComponent : fr::Component
{
    std::vector<fra::MeshHandle>* meshes;
    fra::MaterialHandle           material;
};
