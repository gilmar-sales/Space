# Space Agent Notes

## Build Commands

Use **Ninja + GCC** (not the Visual Studio / MSVC generator):

```bash
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=gcc \
  -DCMAKE_CXX_COMPILER=g++ \
  ..
cmake --build . --parallel
ctest --build-config Release
```

On Windows CI/local, point the compilers at a MinGW-w64 GCC 16+ toolchain (e.g. `C:/mingw64-gcc16/bin/gcc.exe` and `g++.exe`).

## Key Dependencies

- **Vulkan SDK** required (graphics foundation via Freya)
- **xorg** required on Linux (`sudo apt install xorg`)
- External libs fetched via CMake FetchContent: `freyr` (ECS), `freya` (graphics), `skirnir` (DI)

## Compiler Requirements

- C++26 required (Freyr reflection)
- **Must use GCC that supports `-freflection`** (GCC 16+). MSVC and Clang do not implement C++26 reflection and will not build this project.
- MSVC/Windows: `/bigobj` flag (already in CMakeLists.txt) — only relevant if linking MSVC-oriented deps; the app itself builds with GCC.

## Build Artifacts

- `Resources/` directory copied to `${CMAKE_BINARY_DIR}/` during build
- Windows (Ninja/GCC): executable at `build/Space.exe`

## Architecture

- ECS game using Freyr (multithreaded archetype-chunk ECS)
- Freya for rendering/graphics
- Skirnir for IoC dependency injection
- Entry point: `Source/Main.cpp` → `Source/SpaceApp.cpp`
- Components: Transform, RigidBody, Health, Player, Enemy, SpaceShipControl, LaserGun, Model, SphereCollider, Decay
- Events: KeyDown, KeyUp, MouseMove, Collision, OctreeFinished, TransformChange
- Systems: PlayerControl, Movement, Octree, LaserGun, Spawn, EnemyControl, Input, Render, Physics, Collision, Decay
- Containers: Octree, Frustum, ArenaAllocator, LockFreeArray
