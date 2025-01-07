#pragma once

#include <Core/AbstractApplication.hpp>
#include <Freyr/Freyr.hpp>

class SpaceApp final : public fra::AbstractApplication
{
  public:
    void StartUp() override;
    void Update() override;
    void ShutDown() override;

  private:
    std::shared_ptr<fr::Scene> mScene;
};