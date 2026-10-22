#pragma once

#include <Freya/Freya.hpp>

#include <vector>

struct ModelComponent
{
    std::vector<fra::MeshHandle>* meshes;
    fra::MaterialHandle           material;
};
