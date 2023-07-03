cmake_minimum_required(VERSION 3.7)
project(space_hell VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LIVE_PP False)

# Find locally installed dependencies. Tip: Use VCPKG for these.

# Fetch dependencies from Github

include(FetchContent)

FetchContent_Declare(
    BitSquidFoundation
    GIT_REPOSITORY https://github.com/hsjunnesson/bitsquid-foundation.git
)

if (NOT bitsquidfoundation_POPULATED)
    FetchContent_MakeAvailable(BitSquidFoundation)
endif()


add_subdirectory("${CMAKE_SOURCE_DIR}/chocolate")


# Main game source

set(SRC_space_hell
    "src/main.cpp"
    "src/game.h"
    "src/game.cpp"
    "src/game_state_playing.cpp"
    "src/util.h"
    "src/rnd.h"
)

# Create executable
add_executable(${PROJECT_NAME} ${SRC_space_hell})


# Includes

if (LIVE_PP)
    include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/LivePP/API)
endif()


# Linked libraries

target_link_libraries(${PROJECT_NAME} PRIVATE chocolate)


# Compiler warnings & definitions

target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_MATH_DEFINES)

if (CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wno-unknown-pragmas -Wno-gnu-zero-variadic-macro-arguments)
endif()

if (MSVC)
    source_group("foundation" FILES ${bitsquidfoundation_SOURCE_DIR})
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
    set_source_files_properties(${SRC_space_hell} PROPERTIES COMPILE_FLAGS "/W4 /WX /wd4061")

    if (LIVE_PP)
        target_compile_definitions(${PROJECT_NAME} PRIVATE LIVE_PP=1)
        target_compile_options(${PROJECT_NAME} PRIVATE /Zi /Gm-)
        target_link_options(${PROJECT_NAME} PRIVATE /FUNCTIONPADMIN /OPT:NOREF /OPT:NOICF /DEBUG:FULL)
    endif()
endif()
