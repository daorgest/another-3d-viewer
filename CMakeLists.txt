cmake_minimum_required(VERSION 3.20)
project(VulkanEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the path to the 'libs' folder
set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# ImGui
set(IMGUI_DIR "${LIBS_DIR}/imgui") # Path to ImGui source files
file(GLOB IMGUI_SOURCES
        "${IMGUI_DIR}/*.cpp"
        "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
        "${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp")

# Define the source and header files using GLOB
file(GLOB SRC_FILES
        "src/game/*.cpp"
        "src/engine/*.cpp"
        ${IMGUI_SOURCES}
)
file(GLOB HEADER_FILES
        "src/game/*.h"
        "src/game/*.hpp"
        "src/engine/*.h"
        "src/engine/*.hpp"
)

# Add the executable
add_executable(VulkanEngine ${SRC_FILES} ${HEADER_FILES}
        )

# Preprocessor definitions
target_compile_definitions(VulkanEngine PRIVATE
        "$<$<CONFIG:Debug>:WIN32;_DEBUG;_CONSOLE>"
        "$<$<CONFIG:Release>:WIN32;NDEBUG;_CONSOLE>"
)

# Include directories for header files
target_include_directories(${PROJECT_NAME} PRIVATE
        ${LIBS_DIR}/fmt/include
        ${LIBS_DIR}/tinyobjloader
        ${LIBS_DIR}/glfw/include
        ${LIBS_DIR}/glm
        ${LIBS_DIR}/vulkan/include
        ${LIBS_DIR}/VulkanMemoryAllocator/include
        ${IMGUI_DIR}
)

# Vulkan
find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})

# GLFW
target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)
target_link_directories(${PROJECT_NAME} PRIVATE ${LIBS_DIR}/glfw/lib)

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
