cmake_minimum_required(VERSION 3.30)
project(Game2D)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(dependencies/SDL)
add_subdirectory(dependencies/SDL_img)
add_subdirectory(dependencies/SDL_ttf)
add_subdirectory(dependencies/SDL_mixer)

add_executable(${PROJECT_NAME} main.cpp
        src/Player.cpp
        src/Setup.cpp
        src/TextureManager.cpp
        src/Enemy.cpp src/Enemy.h
        src/UI.cpp src/UI.h
        src/Bullet.cpp src/Bullet.h
        src/Map.cpp src/Map.h
        src/TextManager.cpp
        src/TextManager.h
        src/Sound.cpp
        src/Sound.h
        src/Loot.cpp
        src/Loot.h)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf SDL3_mixer::SDL3_mixer)
