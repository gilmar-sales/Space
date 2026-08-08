#include "InputSystem.hpp"

#include <Events/KeyDownEvent.hpp>
#include <Events/KeyUpEvent.hpp>
#include <Events/MouseMoveEvent.hpp>

#include <SDL3/SDL.h>
#include <print>

namespace
{
constexpr const char* QualityName(int index)
{
    static constexpr const char* kNames[] = { "Low", "Medium", "High", "Ultra" };
    return (index >= 0 && index <= 3) ? kNames[index] : "?";
}

template <typename Quality>
Quality NextQuality(Quality current)
{
    switch (current)
    {
        case Quality::Off:
            return Quality::Low;
        case Quality::Low:
            return Quality::Medium;
        case Quality::Medium:
            return Quality::High;
        case Quality::High:
            return Quality::Ultra;
        case Quality::Ultra:
            return Quality::Off;
    }

    return Quality::Low;
}
} // namespace

void InputSystem::PreUpdate(float deltaTime)
{
}

void InputSystem::CycleShadowQuality()
{
    const auto next = NextQuality(mRenderer->GetShadowQuality());
    mRenderer->SetShadowQuality(next);
    std::println("Shadow quality: {} [F5]", QualityName(static_cast<int>(next)));
}

void InputSystem::CycleSsaoQuality()
{
    const auto next = NextQuality(mRenderer->GetSsaoQuality());
    mRenderer->SetSsaoQuality(next);
    std::println("SSAO quality: {} [F6]", QualityName(static_cast<int>(next)));
}

void InputSystem::CycleTaaQuality()
{
    const auto next = NextQuality(mRenderer->GetTaaQuality());
    mRenderer->SetTaaQuality(next);
    std::println("TAA quality: {} [F7]", QualityName(static_cast<int>(next)));
}

void InputSystem::CycleBloomQuality()
{
    const auto next = NextQuality(mRenderer->GetBloomQuality());
    mRenderer->SetBloomQuality(next);
    std::println("Bloom quality: {} [F8]", QualityName(static_cast<int>(next)));
}
