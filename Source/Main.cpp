#include "SpaceApp.hpp"

#include <Freya/Freya.hpp>
#include <Freyr/Builders/SceneBuilder.hpp>

#include "Components/ModelComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/SpaceShipControlComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "Systems/CollisionSystem.hpp"
#include "Systems/InputSystem.hpp"
#include "Systems/MovementSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/PlayerCameraSystem.hpp"
#include "Systems/PlayerControlSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/SpawnSystem.hpp"

int main(int argc, char const* argv[])
{
    auto applicationBuilder =
        fra::ApplicationBuilder()
            .WithWindow([](fra::WindowBuilder& windowBuilder) {
                windowBuilder.SetWidth(1920).SetHeight(1080).SetVSync(false);
            })
            .WithRenderer([](fra::RendererBuilder& rendererBuilder) {
                rendererBuilder.SetSamples(vk::SampleCountFlagBits::e8)
                    .SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f })
                    .SetDrawDistance(100'000.0f)
                    .SetSamples(vk::SampleCountFlagBits::e1);
            });

    applicationBuilder.GetServiceCollection()->AddSingleton<fr::Scene>(
        [&](ServiceProvider& serviceProvider) {
            return fr::SceneBuilder(applicationBuilder.GetServiceCollection())
                .AddComponent<ModelComponent>()
                .AddComponent<TransformComponent>()
                .AddComponent<SphereColliderComponent>()
                .AddComponent<RigidBodyComponent>()
                .AddComponent<PlayerComponent>()
                .AddComponent<SpaceShipControlComponent>()
                .AddSystem<SpawnSystem>()
                .AddSystem<InputSystem>()
                .AddSystem<OctreeSystem>()
                .AddSystem<PlayerControlSystem>()
                .AddSystem<PlayerCameraSystem>()
                .AddSystem<CollisionSystem>()
                .AddSystem<MovementSystem>()
                .AddSystem<PhysicsSystem>()
                .AddSystem<RenderSystem>()
                .WithOptions([](fr::FreyrOptionsBuilder& freyrOptionsBuilder) {
                    freyrOptionsBuilder.SetInitialCapacity(100'000);
                })
                .Build(serviceProvider);
        });

    const auto app = applicationBuilder.Build<SpaceApp>();

    app->Run();

    return 0;
}
