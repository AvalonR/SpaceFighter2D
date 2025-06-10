cmake_minimum_required(VERSION 3.30)
project(Game2D)

set(CMAKE_CXX_STANDARD 20)

set(SDLTTF_VENDORED ON)

add_subdirectory(dependencies/SDL)
add_subdirectory(dependencies/SDL_image)
add_subdirectory(dependencies/SDL_ttf EXCLUDE_FROM_ALL)
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
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    SDL3_mixer::SDL3_mixer
)
if(WIN32)
    # Copy SDL3 DLLs
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL3::SDL3>"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
    
    # Copy SDL3_image DLL
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL3_image::SDL3_image>"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
    
    # Copy SDL3_ttf DLL
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL3_ttf::SDL3_ttf>"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
    
    # Copy SDL3_mixer DLL
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL3_mixer::SDL3_mixer>"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
endif()
