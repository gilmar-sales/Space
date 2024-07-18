#pragma once

#include <Core/AbstractApplication.hpp>
#include <Freyr/Freyr.hpp>

class SpaceApp final : public fra::AbstractApplication
{
  public:
    void Startup();

    void Run() override;

  private:
    std::shared_ptr<fr::Scene> mManager;
};