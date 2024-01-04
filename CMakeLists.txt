cmake_minimum_required(VERSION 3.20)
project(VulkanEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

add_executable(VulkanEngine
        src/game/main.cpp)

# Define the source files using GLOB
file(GLOB SRC_FILES
    "src/game/*.cpp"
    "src/engine/*.cpp"
)

# Define the header files using GLOB
file(GLOB HEADER_FILES
    "src/game/*.h"
    "src/game/*.hpp"
    "src/engine/*.h"
    "src/engine/*.hpp"
)

# Add the source files to the target
target_sources(VulkanEngine PRIVATE ${SRC_FILES} ${HEADER_FILES})

# Preprocessor definitions
target_compile_definitions(VulkanEngine PRIVATE
        "$<$<CONFIG:Debug>:WIN32;_DEBUG;_CONSOLE>"
        "$<$<CONFIG:Release>:WIN32;NDEBUG;_CONSOLE>"
)

# Include directories for header files
target_sources(${PROJECT_NAME} PRIVATE ${SRC_FILES})

# Set the path to the 'libs' folder
set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# Vulkan
find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})

# GLFW
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)
else()
    find_package(glfw3 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
endif()

# FMT
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBS_DIR}/fmt/include)

# TinyObjLoader
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBS_DIR}/tinyobjloader)

# GLFW/include
target_link_directories(${PROJECT_NAME} PRIVATE ${LIBS_DIR}/glfw/lib)
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBS_DIR}/glfw/include)

# GLM include
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBS_DIR}/glm)

# Vulkan include
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBS_DIR}/vulkan/include)

# Find GLSL Validator
find_program(GLSL_VALIDATOR glslangValidator HINTS
    ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
    /usr/bin
    /usr/local/bin
    ${VULKAN_SDK_PATH}/Bin
    ${VULKAN_SDK_PATH}/Bin32
    $ENV{VULKAN_SDK}/Bin/
    $ENV{VULKAN_SDK}/Bin32/
)

# Get all .vert and .frag files in the shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/src/game/shaders/*.frag"
    "${PROJECT_SOURCE_DIR}/src/game/shaders/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

# Create a custom target for shaders
add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)

# Add the Shaders target as a dependency for the main executable
add_dependencies(${PROJECT_NAME} Shaders)
