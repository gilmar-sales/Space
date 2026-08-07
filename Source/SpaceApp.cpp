#include "SpaceApp.hpp"

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
}

void SpaceApp::Update()
{
    mRegistry->Update(mWindow->GetDeltaTime());
}

void SpaceApp::ShutDown()
{
}
