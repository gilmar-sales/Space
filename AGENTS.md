# Space Agent Notes

## Build Commands
```bash
mkdir build && cd build && cmake ..
cmake --build . --parallel
ctest --build-config Release
```

## Key Dependencies
- **Vulkan SDK** required (graphics foundation via Freya)
- **xorg** required on Linux (`sudo apt install xorg`)
- External libs fetched via CMake FetchContent: `freyr` (ECS), `freya` (graphics), `skirnir` (DI)

## Compiler Requirements
- C++26 required (Freyr reflection)
- MSVC/Windows: `/bigobj` flag (already in CMakeLists.txt)
- GCC 16+ / Clang 22+ recommended (reflection support)

## Build Artifacts
- `Resources/` directory copied to `${CMAKE_BINARY_DIR}/` during build
- Windows: executable at `build/Release/Space.exe`

## Architecture
- ECS game using Freyr (multithreaded archetype-chunk ECS)
- Freya for rendering/graphics
- Skirnir for IoC dependency injection
- Entry point: `Source/Main.cpp` → `Source/SpaceApp.cpp`
- Components: Transform, RigidBody, Health, Player, Enemy, SpaceShipControl, LaserGun, Model, SphereCollider, Decay
- Events: KeyDown, KeyUp, MouseMove, Collision, OctreeFinished, TransformChange
- Systems: PlayerControl, Movement, Octree, LaserGun, Spawn, EnemyControl, Input, Render, Physics, Collision, Decay
- Containers: Octree, Frustum, ArenaAllocator, LockFreeArray
