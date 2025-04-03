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
        fra::ApplicationBuilder().WithOptions([](fra::FreyaOptions& options) {
            options.title        = "Space";
            options.width        = 1920;
            options.height       = 1080;
            options.drawDistance = 1000'000.0f;
            options.sampleCount  = 8;
            options.vSync        = false;
        });

    applicationBuilder.GetServiceCollection()->AddSingleton<fr::Scene>(
        [&](skr::ServiceProvider& serviceProvider) {
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
                .Build(serviceProvider);
        });

    const auto app = applicationBuilder.Build<SpaceApp>();

    app->Run();

    return 0;
}
