cmake_minimum_required(VERSION 3.20)

add_subdirectory(ThirdParty/Freyr)
add_subdirectory(ThirdParty/Freya)

project(
    SpaceAiWar
    LANGUAGES C CXX
    )

file(GLOB_RECURSE SPACE_SRC Source/**.cpp Source/**.hpp)

add_executable(SpaceAiWar ${SPACE_SRC})

target_compile_features(SpaceAiWar PUBLIC cxx_std_23)

target_link_libraries(SpaceAiWar freyr::freyr Freya-static)

target_include_directories(SpaceAiWar PUBLIC Source)

target_precompile_headers(SpaceAiWar PUBLIC
  <memory>
  )

if(MSVC)
  target_compile_options(SpaceAiWar PUBLIC "/ZI" "/MP")
  target_link_options(SpaceAiWar PUBLIC "/INCREMENTAL")
endif()

file(COPY Resources DESTINATION ${CMAKE_BINARY_DIR})