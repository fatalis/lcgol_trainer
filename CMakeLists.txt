cmake_minimum_required(VERSION 3.3)
project(lcgol_trainer)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Werror -O2 -Wl,--enable-stdcall-fixup")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -O2 -Wl,--enable-stdcall-fixup")
set(CMAKE_CXX_STANDARD 14)

add_link_options(-static)

set(SUBHOOK_STATIC ON)
set(SUBHOOK_TESTS OFF)
add_subdirectory(subhook)
link_directories(minidx9/Lib/x86)
include_directories(
    minidx9/Include
    hacklib/src/hacklib/include
)

set(SOURCE_FILES
    hacklib/src/hacklib/src/Drawer.cpp

    src/dllmain.cpp
    src/DebugMenu.cpp
    src/EntityViewer.cpp
    src/Game.cpp
    src/Hooks.cpp
    src/Hud.cpp
    src/Input.cpp
    src/Menu.cpp
    src/Trainer.cpp

    src/wrapper.cpp
    src/dll.def
)

add_library(lcgol_trainer SHARED ${SOURCE_FILES})

target_link_libraries(lcgol_trainer
    d3dx9
    XInput
    subhook
)

set_target_properties(lcgol_trainer PROPERTIES PREFIX "")
set_target_properties(lcgol_trainer PROPERTIES OUTPUT_NAME "version")

if(DEFINED ENV{USER} AND "$ENV{USER}" STREQUAL "fatalis")
    add_custom_command(
        TARGET lcgol_trainer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:lcgol_trainer> '/home/fatalis/games/Lara Croft and the Guardian of Light/'
        COMMENT "Copying $<TARGET_FILE:lcgol_trainer> to output directory"
    )
endif()
