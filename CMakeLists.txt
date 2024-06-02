cmake_minimum_required(VERSION 3.3)
project(lcgol_trainer)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -fvisibility=hidden")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)

add_link_options(-static)

set(SUBHOOK_STATIC ON)
set(SUBHOOK_TESTS OFF)
add_subdirectory(libs/subhook)
link_directories(libs/minidx9/Lib/x86)
include_directories(
    libs/Deviare-InProc/Include
    libs/minidx9/Include
    libs/hacklib/src/hacklib/include
)

set(SOURCE_FILES
    libs/hacklib/src/hacklib/src/Drawer.cpp

    src/dllmain.cpp
    src/DebugMenu.cpp
    src/EntityViewer.cpp
    src/Game.cpp
    src/Hooks.cpp
    src/Hud.cpp
    src/Input.cpp
    src/Menu.cpp
    src/Trainer.cpp
    # src/wrapper.cpp
)

add_library(lcgol_trainer SHARED ${SOURCE_FILES})

target_link_libraries(lcgol_trainer
    d3dx9
    XInput
    subhook
)

set_target_properties(lcgol_trainer PROPERTIES PREFIX "")
# set_target_properties(lcgol_trainer PROPERTIES OUTPUT_NAME "version")
set_target_properties(lcgol_trainer PROPERTIES OUTPUT_NAME "trainer")

add_custom_command(
    TARGET lcgol_trainer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:lcgol_trainer> '/home/fatalis/games/Lara Croft and the Guardian of Light2/'
    COMMENT "Copying MyExecutable to output directory"
)
