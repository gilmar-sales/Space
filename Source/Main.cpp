#include "SpaceApp.hpp"

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

#include "Components/HealthComponent.hpp"
#include "Components/LaserGunComponent.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/SpaceShipControlComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/SquadComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "Systems/AIControlSystem.hpp"
#include "Systems/CollisionSystem.hpp"
#include "Systems/DecaySystem.hpp"
#include "Systems/InputSystem.hpp"
#include "Systems/LaserGunSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/PlayerControlSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/SpaceShipSystem.hpp"
#include "Systems/SpawnSystem.hpp"

#include "Random.hpp"

int main(int argc, char const* argv[])
{
    auto builder =
        skr::ApplicationBuilder()
            .AddExtension<fr::FreyrExtension>([](Ref<fr::FreyrExtension> freyr) {
                freyr
                    ->WithOptions([](fr::FreyrOptionsBuilder& options) {
                        options.WithArchetypeChunkCapacity(128).WithThreadCount(std::thread::hardware_concurrency());
                    })
                    .WithComponent<ModelComponent>()
                    .WithComponent<TransformComponent>()
                    .WithComponent<SphereColliderComponent>()
                    .WithComponent<RigidBodyComponent>()
                    .WithComponent<HealthComponent>()
                    .WithComponent<SquadComponent>()
                    .WithComponent<PlayerComponent>()
                    .WithComponent<SpaceShipControlComponent>()
                    .WithComponent<LaserGunComponent>()
                    .WithPipeline([](fr::PipelineBuilder& pipeline) {
                        pipeline.WithName("Main")
                            .WithSystem<SpawnSystem>()
                            .WithSystem<InputSystem>()
                            .WithSystem<PlayerControlSystem>()
                            .WithSystem<LaserGunSystem>()
                            .WithSystem<RenderSystem>();
                    })
                    .WithPipeline([](fr::PipelineBuilder& pipeline) {
                        pipeline.WithName("Fixed")
                            .WithRate(60.0f)
                            .WithSystem<OctreeSystem>()
                            .WithSystem<SpaceShipSystem>()
                            .WithSystem<PhysicsSystem>()
                            .WithSystem<CollisionSystem>();
                    })
                    .WithPipeline([](fr::PipelineBuilder& pipeline) {
                        pipeline.WithName("AI").WithRate(5.0f).WithSystem<AIControlSystem>().WithSystem<DecaySystem>();
                    });
            })
            .AddExtension<fra::FreyaExtension>([](Ref<fra::FreyaExtension> freya) {
                freya->WithOptions([](fra::FreyaOptionsBuilder& freyaOptions) {
                    freyaOptions.SetTitle("Space")
                        .SetWidth(1920)
                        .SetHeight(1080)
                        .SetDrawDistance(1000'000.0f)
                        .SetSampleCount(8)
                        .SetVSync(false)
                        .SetFullscreen(false);
                });
            });

    builder.GetServiceCollection()->AddSingleton<AssetManager>();
    builder.GetServiceCollection()->AddTransient<Random>();

    auto app = builder.Build<SpaceApp>();

    app->Run();

    return 0;
}
