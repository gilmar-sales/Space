#pragma once

#include <random>

class Random
{
  public:
    Random() : e1(r()) {}

    float Float(float min, float max)
    {
        std::uniform_real_distribution<float> float_dis(min, max);

        return float_dis(e1);
    }

    int32_t Int(int32_t min, int32_t max)
    {
        std::uniform_int_distribution<int32_t> int_dis(min, max);
        return int_dis(e1);
    }

    glm::vec3 Position(float min, float max) { return glm::vec3 { Float(min, max), Float(min, max), Float(min, max) }; }

    glm::vec3 PositionFrom(glm::vec3 origin, float min, float max)
    {
        return origin + glm::vec3 { Float(min, max), Float(min, max), Float(min, max) };
    }

  private:
    std::random_device         r;
    std::default_random_engine e1;
};