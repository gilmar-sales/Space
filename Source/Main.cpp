#include "Components/LaserGunComponent.hpp"
#include "SpaceApp.hpp"

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

#include "Components/ModelComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/SpaceShipControlComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "Systems/CollisionSystem.hpp"
#include "Systems/DecaySystem.hpp"
#include "Systems/EnemyControlSystem.hpp"
#include "Systems/InputSystem.hpp"
#include "Systems/LaserGunSystem.hpp"
#include "Systems/MovementSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/PlayerControlSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/SpawnSystem.hpp"

int main(int argc, char const* argv[])
{
    auto app = skr::ApplicationBuilder()
                   .AddExtension<fr::FreyrExtension>([](fr::FreyrExtension& freyr) {
                       freyr
                           .WithOptions([](fr::FreyrOptionsBuilder& options) {
                               options.SetMaxEntities(512 * 1024).SetArchetypeChunkCapacity(1024).SetThreadCount(10);
                           })
                           .AddComponent<ModelComponent>()
                           .AddComponent<TransformComponent>()
                           .AddComponent<SphereColliderComponent>()
                           .AddComponent<RigidBodyComponent>()
                           .AddComponent<PlayerComponent>()
                           .AddComponent<SpaceShipControlComponent>()
                           .AddComponent<LaserGunComponent>()
                           .AddSystem<SpawnSystem>()
                           .AddSystem<DecaySystem>()
                           .AddSystem<InputSystem>()
                           .AddSystem<OctreeSystem>()
                           .AddSystem<PlayerControlSystem>()
                           .AddSystem<EnemyControlSystem>()
                           .AddSystem<LaserGunSystem>()
                           .AddSystem<CollisionSystem>()
                           .AddSystem<MovementSystem>()
                           .AddSystem<PhysicsSystem>()
                           .AddSystem<RenderSystem>();
                   })
                   .AddExtension<fra::FreyaExtension>([](fra::FreyaExtension& freya) {
                       freya.WithOptions([](fra::FreyaOptionsBuilder& freyaOptions) {
                           freyaOptions.SetTitle("Space")
                               .SetWidth(1920)
                               .SetHeight(1080)
                               .SetDrawDistance(1000'000.0f)
                               .SetSampleCount(8)
                               .SetVSync(false)
                               .SetFullscreen(false);
                       });
                   })
                   .Build<SpaceApp>();

    app->Run();

    return 0;
}
