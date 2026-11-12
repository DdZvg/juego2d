cmake_minimum_required(VERSION 3.24)
project(juego LANGUAGES CXX)

find_package(SFML 3 REQUIRED COMPONENTS Graphics)

add_library(game_core
    src/Player.cpp
    src/Camera.cpp
    src/World.cpp
    src/InputManager.cpp
    src/HealthComponent.cpp
    src/Enemy.cpp
    src/Projectile.cpp
    src/CombatSystem.cpp
)
target_include_directories(game_core PUBLIC src)
target_compile_features(game_core PUBLIC cxx_std_17)
target_link_libraries(game_core PUBLIC SFML::Graphics)
set_target_properties(game_core PROPERTIES CXX_EXTENSIONS OFF)

add_executable(juego src/main.cpp src/Game.cpp)
target_compile_features(juego PRIVATE cxx_std_17)
target_link_libraries(juego PRIVATE game_core)
set_target_properties(juego PROPERTIES CXX_EXTENSIONS OFF)

include(CTest)
if(BUILD_TESTING)
    add_executable(game_tests tests/GameTests.cpp)
    target_link_libraries(game_tests PRIVATE game_core)
    add_test(NAME game_tests COMMAND game_tests)
endif()
