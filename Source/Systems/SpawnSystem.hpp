#pragma once

#include <Freyr.hpp>

class SpawnSystem : public fr::System
{
    public:
        void Start() override;
        void Update(float dt) override;
};