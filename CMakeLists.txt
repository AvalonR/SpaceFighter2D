cmake_minimum_required(VERSION 3.30)
project(Game2D)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-D_FILE_OFFSET_BITS=64)
# Keep your game in Debug
set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)

set(SDLMIXER_VENDORED ON)
set(SDLTTF_VENDORED ON)

# Save project build type
set(MY_BUILD_TYPE ${CMAKE_BUILD_TYPE})

# Force dependencies to Release to avoid huge obj files
set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
add_subdirectory(dependencies/SDL)
add_subdirectory(dependencies/SDL_image)
add_subdirectory(dependencies/SDL_ttf EXCLUDE_FROM_ALL)
add_subdirectory(dependencies/SDL_mixer)
# Force Harfbuzz to build without debug info
if (TARGET harfbuzz)
    message(STATUS "Stripping debug info from Harfbuzz...")
    target_compile_options(harfbuzz PRIVATE -O2 -DNDEBUG -g0)
endif()
# Restore your game’s build type (Debug)
set(CMAKE_BUILD_TYPE ${MY_BUILD_TYPE} CACHE STRING "" FORCE)

add_executable(${PROJECT_NAME}
    main.cpp
    src/Player.cpp
    src/Setup.cpp
    src/TextureManager.cpp
    src/Enemy.cpp src/Enemy.h
    src/UI.cpp src/UI.h
    src/Bullet.cpp src/Bullet.h
    src/Map.cpp src/Map.h
    src/TextManager.cpp src/TextManager.h
    src/Sound.cpp src/Sound.h
    src/Loot.cpp src/Loot.h
)

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
