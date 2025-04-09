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
#include "Systems/PlayerControlSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/SpawnSystem.hpp"

int main(int argc, char const* argv[])
{
    auto applicationBuilder = fra::ApplicationBuilder().WithOptions(
        [](fra::FreyaOptionsBuilder& options) {
            options.SetTitle("Space")
                .SetWidth(1920)
                .SetHeight(1080)
                .SetDrawDistance(1000'000.0f)
                .SetSampleCount(8)
                .SetVSync(false)
                .SetFullscreen(false);
        });

    applicationBuilder.GetServiceCollection()->AddSingleton<fr::Scene>(
        [&](skr::ServiceProvider& serviceProvider) {
            return fr::SceneBuilder(applicationBuilder.GetServiceCollection())
                .WithOptions([](fr::FreyrOptionsBuilder& options) {
                    options.SetMaxEntities(512 * 1024);
                })
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
