cmake_minimum_required(VERSION 3.10)
project(VulkanEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the build type (Debug or Release)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Define the target executable
add_executable(VulkanEngine
        src/engine/oeg_utils.h)

# Source files for the target
file(GLOB_RECURSE SRC_FILES
        "src/*.cpp"
        "src/*.h"
)

# Add the source files to the target
target_sources(VulkanEngine PRIVATE ${SRC_FILES})

# Include directories for the target
target_include_directories(VulkanEngine PRIVATE
        "${CMAKE_SOURCE_DIR}/libs/fmt/include"
        "${CMAKE_SOURCE_DIR}/libs/tinyobjloader"
        "${CMAKE_SOURCE_DIR}/libs/glfw/include"
        "${CMAKE_SOURCE_DIR}/libs/glm"
#        "${CMAKE_SOURCE_DIR}/libs/VulkanMemoryAllocator/include"
        "C:/VulkanSDK/1.3.250.0/Include"
)

# Preprocessor definitions
target_compile_definitions(VulkanEngine PRIVATE
        "$<$<CONFIG:Debug>:WIN32;_DEBUG;_CONSOLE>"
        "$<$<CONFIG:Release>:WIN32;NDEBUG;_CONSOLE>"
)

# Link libraries
if(WIN32)
    target_link_directories(VulkanEngine PRIVATE
            "${CMAKE_SOURCE_DIR}/libs/glfw/lib"
            "C:/VulkanSDK/1.3.250.0/Lib"
    )

    target_link_libraries(VulkanEngine PRIVATE
            vulkan-1
            glfw3
    )
elseif(APPLE)
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
    find_library(CORE_VIDEO_FRAMEWORK CoreVideo REQUIRED)

    target_link_libraries(VulkanEngine PRIVATE
            "-framework Vulkan"
            "-framework Metal"
            glfw3
            ${COCOA_FRAMEWORK}
            ${IOKIT_FRAMEWORK}
            ${CORE_FOUNDATION_FRAMEWORK}
            ${CORE_VIDEO_FRAMEWORK}
    )
else() # Linux
    find_package(Vulkan REQUIRED)
    find_package(glfw3 REQUIRED)

    target_link_libraries(VulkanEngine PRIVATE
            ${Vulkan_LIBRARIES}
            glfw
    )
endif()

# Set the output directory for the executable
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")

# Compile shaders
# TODO: Will make it so I dont have to specify exact directories
add_custom_command(TARGET VulkanEngine PRE_BUILD
        COMMAND "C:/VulkanSDK/1.3.250.0/Bin/glslc.exe" "${CMAKE_CURRENT_SOURCE_DIR}/src/game/shaders/simple_shader.vert" -o "${CMAKE_CURRENT_SOURCE_DIR}/src/game/shaders/simple_shader.vert.spv"
        COMMAND "C:/VulkanSDK/1.3.250.0/Bin/glslc.exe" "${CMAKE_CURRENT_SOURCE_DIR}/src/game/shaders/simple_shader.frag" -o "${CMAKE_CURRENT_SOURCE_DIR}/src/game/shaders/simple_shader.frag.spv"
        COMMENT "Building Shaders..."
)

# Copy the models and shaders directories to bin
add_custom_command(TARGET VulkanEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/src/game/models"
        "${CMAKE_SOURCE_DIR}/bin/models"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/src/game/shaders"
        "${CMAKE_SOURCE_DIR}/bin/shaders"
        COMMENT "Copying models and shaders directories..."
)
