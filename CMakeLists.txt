# This CMake script is designed for CLion IDE and QTCreator projects on Windows
# It should find bundled binary dependencies automatically when using MinGW compiler
# On Linux and Mac the dependencies (glm, glew, glfw) need to be installed manually

cmake_minimum_required(VERSION 3.1)
project(ppgso CXX)

#
# CONFIGURATION
#

# Basic CMake settings
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/_install)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data)

# Use add_resources function to convert .glsl to a .cpp files which will be compiled into a separate static library
include(add_resources)

#
# DEPENDENCIES
#

# Set up external dependencies for Windows users
if (MINGW)
  set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${CMAKE_SOURCE_DIR}/dependencies/include/")
  set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${CMAKE_SOURCE_DIR}/dependencies/lib/mingw")
elseif (MSVC)
  set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${CMAKE_SOURCE_DIR}/dependencies/include/")
  if(MSVC_VERSION == 1500)
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${CMAKE_SOURCE_DIR}/dependencies/lib/vc2015")
  elseif(MSVC_VERSION >= 1900)
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${CMAKE_SOURCE_DIR}/dependencies/lib/vc2019")
  endif()
  set(USE_STRICT_COMPILE_WARNINGS OFF CACHE BOOL "" FORCE)
endif ()

# Warnings for Debug mode
option(USE_STRICT_COMPILE_WARNINGS "Use strict compilation warnings in debug mode." ON)
# These compile flags should apply for GCC and Clang compilers
set(STRICT_COMPILE_FLAGS "-Wpedantic -Wall -Wno-c++98-compat -Wextra -Wno-sign-conversion -Wno-unused-parameter")
if (USE_STRICT_COMPILE_WARNINGS)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${STRICT_COMPILE_FLAGS}")
endif ()

# Find required packages
find_package(GLFW3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(OpenGL REQUIRED)

# Optional packages
find_package(OpenMP)
if(OPENMP_FOUND)
  list(APPEND CMAKE_CXX_FLAGS ${OpenMP_CXX_FLAGS})
endif()

# Set default installation destination
if (NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "../_install")
endif ()

#
# Shaders / Materials
#

set(PPGSO_SHADER_SRC
        shader/color_vert.glsl shader/color_frag.glsl
        shader/convolution_vert.glsl shader/convolution_frag.glsl
        shader/diffuse_vert.glsl shader/diffuse_frag.glsl
        shader/texture_vert.glsl shader/texture_frag.glsl
        shader/phong_vert_glsl.glsl shader/phong_frag_glsl.glsl
        shader/particle_vert.glsl shader/particle_frag.glsl
        shader/framebuffer_vert.glsl shader/framebuffer_frag.glsl
        )
add_resources(shaders ${PPGSO_SHADER_SRC})

# PPGSO library
add_library(ppgso STATIC
        ppgso/mesh.cpp
        ppgso/tiny_obj_loader.cpp
        ppgso/shader.cpp
        ppgso/image.cpp
        ppgso/image_bmp.cpp
        ppgso/image_raw.cpp
        ppgso/texture.cpp
        ppgso/window.cpp
        )

# Make sure GLM uses radians and GLEW is a static library
target_compile_definitions(ppgso PUBLIC -DGLM_FORCE_RADIANS -DGLEW_STATIC)

# Link to GLFW, GLEW and OpenGL
target_link_libraries(ppgso PUBLIC ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
# Pass on include directories
target_include_directories(ppgso PUBLIC
        ppgso
        ${GLFW_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR})

#
# TARGETS
#

# raw1_gradient
add_executable(raw1_gradient src/raw1_gradient/raw1_gradient.cpp)
target_link_libraries(raw1_gradient ppgso)
install(TARGETS raw1_gradient DESTINATION .)

# raw2_raycast
add_executable(raw2_raycast src/raw2_raycast/raw2_raycast.cpp)
target_link_libraries(raw2_raycast ppgso)
install(TARGETS raw2_raycast DESTINATION .)

# raw3_raytrace
add_executable(raw3_raytrace src/raw3_raytrace/raw3_raytrace.cpp)
target_link_libraries(raw3_raytrace ppgso ${OpenMP_libomp_LIBRARY})
install(TARGETS raw3_raytrace DESTINATION .)

# raw4_raster
add_executable(raw4_raster src/raw4_raster/raw4_raster.cpp)
target_link_libraries(raw4_raster ppgso)
install(TARGETS raw4_raster DESTINATION .)

# gl1_gradient
add_executable(gl1_gradient src/gl1_gradient/gl1_gradient.cpp)
target_link_libraries(gl1_gradient ppgso shaders)
install(TARGETS gl1_gradient DESTINATION .)

# gl2_texture
add_executable(gl2_texture src/gl2_texture/gl2_texture.cpp)
target_link_libraries(gl2_texture ppgso shaders)
install(TARGETS gl2_texture DESTINATION .)

# gl3_animate
add_executable(gl3_animate src/gl3_animate/gl3_animate.cpp)
target_link_libraries(gl3_animate ppgso shaders ${OpenMP_libomp_LIBRARY})
install(TARGETS gl3_animate DESTINATION .)

# gl4_transform
add_executable(gl4_transform src/gl4_transform/gl4_transform.cpp)
target_link_libraries(gl4_transform ppgso shaders)
install(TARGETS gl4_transform DESTINATION .)

# gl5_projection
add_executable(gl5_projection src/gl5_projection/gl5_projection.cpp)
target_link_libraries(gl5_projection ppgso shaders)
install(TARGETS gl5_projection DESTINATION .)

# gl6_mesh
add_executable(gl6_mesh src/gl6_mesh/gl6_mesh.cpp)
target_link_libraries(gl6_mesh ppgso shaders)
install(TARGETS gl6_mesh DESTINATION .)

# gl7_diffuse
add_executable(gl7_diffuse src/gl7_diffuse/gl7_diffuse.cpp)
target_link_libraries(gl7_diffuse ppgso shaders)
install(TARGETS gl7_diffuse DESTINATION .)

# gl8_framebuffer
add_executable(gl8_framebuffer src/gl8_framebuffer/gl8_framebuffer.cpp)
target_link_libraries(gl8_framebuffer ppgso shaders)
install(TARGETS gl8_framebuffer DESTINATION .)

# gl9_scene
add_executable(gl9_scene
        src/gl9_scene/gl9_scene.cpp
        src/gl9_scene/object.cpp
        src/gl9_scene/scene.cpp
        src/gl9_scene/camera.cpp
        src/gl9_scene/asteroid.cpp
        src/gl9_scene/generator.cpp
        src/gl9_scene/player.cpp
        src/gl9_scene/projectile.cpp
        src/gl9_scene/explosion.cpp
        src/gl9_scene/space.cpp)
target_link_libraries(gl9_scene ppgso shaders)
install(TARGETS gl9_scene DESTINATION .)
add_custom_command(TARGET gl9_scene POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data/ ${CMAKE_CURRENT_BINARY_DIR})

#Project
add_executable(project
        src/project/Camera.cpp
        src/project/objects/Cube.cpp
        src/project/objects/Duck.cpp
        src/project/objects/Particle.cpp
        src/project/Model.cpp
        src/project/Object.cpp
        src/project/project.cpp
        src/project/Scene.cpp
        src/project/SceneManager.cpp
        src/project/scenes/GeneralScene.cpp
        src/project/scenes/AlleyScene.cpp
        src/project/scenes/DiscoScene.cpp
        src/project/characters/Head.cpp
        src/project/characters/Steve.cpp
        src/project/ParticleGenerator.cpp
        src/project/LightSource.cpp
        src/project/objects/ParticleSystem.cpp
        src/project/objects/PureParticle.cpp
        src/project/objects/Drip.cpp
        src/project/objects/Floor.cpp
        src/project/ThrowedItemGenerator.cpp)
target_link_libraries(project ppgso shaders)
install(TARGETS project DESTINATION .)
add_custom_command(TARGET project POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data/ ${CMAKE_CURRENT_BINARY_DIR})


# TASKs

# task1_filter
add_executable(task1_filter src/task1_filter/task1_filter.cpp)
install(TARGETS task1_filter DESTINATION .)

# task2_bresenham
add_executable(task2_bresenham src/task2_bresenham/task2_bresenham.cpp)
target_link_libraries(task2_bresenham ppgso shaders)
install(TARGETS task2_bresenham DESTINATION .)

# task3_bezier
add_executable(task3_bezier src/task3_bezier/task3_bezier.cpp)
target_link_libraries(task3_bezier ppgso shaders)
install(TARGETS task3_bezier DESTINATION .)

# task4_2dshapes
add_executable(task4_2dshapes src/task4_2dshapes/task4_2dshapes.cpp)
target_link_libraries(task4_2dshapes ppgso shaders)
install(TARGETS task4_2dshapes DESTINATION .)

# task5_3d_origin
add_executable(task5_3d_origin src/task5_3d_origin/task5_3d_origin.cpp)
target_link_libraries(task5_3d_origin ppgso shaders)
install(TARGETS task5_3d_origin DESTINATION .)
add_custom_command(TARGET task5_3d_origin POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data/ ${CMAKE_CURRENT_BINARY_DIR})

# task6_bezier_surface
add_executable(task6_bezier_surface src/task6_bezier_surface/task6_bezier_surface.cpp)
target_link_libraries(task6_bezier_surface ppgso shaders)
install(TARGETS task6_bezier_surface DESTINATION .)

# task7_particles
add_executable(task7_particles src/task7_particles/task7_particles.cpp)
target_link_libraries(task7_particles ppgso shaders)
install(TARGETS task7_particles DESTINATION .)
add_custom_command(TARGET task7_particles POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data/ ${CMAKE_CURRENT_BINARY_DIR})

# Playground target
add_executable(playground src/playground/playground.cpp)
target_link_libraries(playground ppgso shaders)
install (TARGETS playground DESTINATION .)
add_custom_command(TARGET playground POST_BUILD COMMAND  ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data/ ${CMAKE_CURRENT_BINARY_DIR})
#
# INSTALLATION
#

file(COPY "data/" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
install(DIRECTORY data/ DESTINATION .)
