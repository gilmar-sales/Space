#include "SpaceApp.hpp"

#include "Systems/CollisionSystem.hpp"
#include "Systems/InputSystem.hpp"
#include "Systems/PlayerControlSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/SpawnSystem.hpp"

#include <SDL3/SDL.h>

#include <cstdio>

SpaceApp::SpaceApp(const skr::Arc<skr::ServiceProvider>& serviceProvider,
                   const skr::Arc<fr::Registry>&         registry) :
    AbstractApplication(serviceProvider), mRegistry(registry)
{
    WarmFreyaBoundSystems();
}

void SpaceApp::WarmFreyaBoundSystems()
{
    const auto sp = GetMainServiceProvider();

    // Spawn first so FindUnique<PlayerComponent>() succeeds in system ctors.
    (void)sp->GetService<SpawnSystem>();

    (void)sp->GetService<RenderSystem>();
    (void)sp->GetService<InputSystem>();
    (void)sp->GetService<PlayerControlSystem>();
    (void)sp->GetService<CollisionSystem>();
}

void SpaceApp::StartUp()
{
    SDL_AddGamepadMappingsFromFile("./Resources/gamecontrollerdb.txt");

    auto gamepadCount = 0;
    auto gamepads     = SDL_GetGamepads(&gamepadCount);
    if (gamepadCount > 0)
    {
        auto gamepad = SDL_OpenGamepad(gamepads[0]);
        if (gamepad)
        {
            std::printf("Found gamepad %s\n", SDL_GetGamepadName(gamepad));
        }
    }
    SDL_free(gamepads);

    // LightService is window-scoped; seed a key light so deferred isn't unlit.
    auto lights = GetMainServiceProvider()->GetService<fra::LightService>();
    auto key    = fra::MakeDirectionalLight(glm::vec3(-0.35f, -1.0f, -0.25f),
                                            glm::vec3(1.0f, 0.97f, 0.92f), 2.5f);
    key.castShadows = false;
    lights->AddLight(key);
}

void SpaceApp::Update()
{
    mRegistry->Update(mWindow->GetDeltaTime());
}

void SpaceApp::ShutDown()
{
}
