#include "SpaceApp.hpp"

#include "Freyr/Core/Scene.hpp"

void SpaceApp::StartUp()
{
    SDL_AddGamepadMappingsFromFile("./Resources/gamecontrollerdb.txt");

    auto gamepadCount = 0;
    auto gamepads     = SDL_GetGamepads(&gamepadCount);
    if (gamepadCount > 0)
    {
        auto gamepad = SDL_OpenGamepad(gamepads[1]);
        if (gamepad)
        {
            std::printf("Found gamepad %s\n", SDL_GetGamepadName(gamepad));
        }
    }
}

void SpaceApp::Update()
{
    mScene->Update(mWindow->GetDeltaTime());
}

void SpaceApp::ShutDown()
{
}
