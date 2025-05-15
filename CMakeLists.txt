cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
  freyr
  GIT_REPOSITORY "git@github.com:gilmar-sales/Freyr.git"
  GIT_TAG        "v0.12.0"
)
FetchContent_MakeAvailable(freyr)

FetchContent_Declare(
  freya
  GIT_REPOSITORY "https://github.com/gilmar-sales/Freya.git"
  GIT_TAG        "v0.8.4"
)
FetchContent_MakeAvailable(freya)

project(
        SpaceAiWar
        LANGUAGES C CXX
)

cmake_policy(SET CMP0002 NEW)

file(GLOB_RECURSE SPACE_SRC Source/**.cpp Source/**.hpp)

add_executable(SpaceAiWar ${SPACE_SRC})

target_compile_features(SpaceAiWar PUBLIC cxx_std_23)

target_link_libraries(SpaceAiWar freyr::freyr Freya::Freya)

target_include_directories(SpaceAiWar PUBLIC Source)

target_precompile_headers(SpaceAiWar PUBLIC
        <memory>
)
if (WIN32)
  if (MSVC)
  target_compile_options(SpaceAiWar PUBLIC "/bigobj")
  else()
  target_compile_options(SpaceAiWar PUBLIC "-Wa,-mbig-obj")
  target_compile_options(SpaceAiWar PRIVATE "-Os")
  endif()
endif (WIN32)

if (MSVC)
    target_compile_options(SpaceAiWar PUBLIC "/ZI" "/MP")
    target_link_options(SpaceAiWar PUBLIC "/INCREMENTAL")
endif ()

file(COPY Resources DESTINATION ${CMAKE_BINARY_DIR})