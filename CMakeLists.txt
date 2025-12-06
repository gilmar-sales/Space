cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
        freyr
        GIT_REPOSITORY "https://github.com/gilmar-sales/Freyr.git"
        GIT_TAG "v0.19.8"
)
FetchContent_MakeAvailable(freyr)

FetchContent_Declare(
        freya
        GIT_REPOSITORY "https://github.com/gilmar-sales/Freya.git"
        GIT_TAG "v0.11.4"
)
FetchContent_MakeAvailable(freya)

project(
        Space
        LANGUAGES C CXX
)

cmake_policy(SET CMP0002 NEW)

file(GLOB_RECURSE SPACE_SRC Source/**.cpp Source/**.hpp)

add_executable(Space ${SPACE_SRC})

target_compile_features(Space PUBLIC cxx_std_23)

target_link_libraries(Space freyr::freyr Freya::Freya)

target_include_directories(Space PUBLIC Source)

target_precompile_headers(Space PUBLIC
        <memory>
)
if (WIN32)
    if (MSVC)
        target_compile_options(Space PUBLIC "/bigobj")
    else ()
        target_compile_options(Space PUBLIC "-Wa,-mbig-obj")
        target_compile_options(Space PRIVATE "-Os")
    endif ()
endif (WIN32)

if (MSVC)
    target_compile_options(Space PUBLIC "/ZI" "/MP")
    target_link_options(Space PUBLIC "/INCREMENTAL")
endif ()

file(COPY Resources DESTINATION ${CMAKE_BINARY_DIR})