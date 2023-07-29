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
elseif (UNIX)
    message(STATUS "CREATING BUILD FOR UNIX")
    target_include_directories(${PROJECT_NAME} PUBLIC
            ${PROJECT_SOURCE_DIR}/src
            ${TINYOBJ_PATH}
    )
    target_link_libraries(${PROJECT_NAME} glfw ${Vulkan_LIBRARIES})
endif ()
# Set the output directory for the executable
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")

find_program(GLSL_VALIDATOR glslangValidator HINTS
        ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        /usr/bin
        /usr/local/bin
        ${VULKAN_SDK_PATH}/Bin
        ${VULKAN_SDK_PATH}/Bin32
        $ENV{VULKAN_SDK}/Bin/
        $ENV{VULKAN_SDK}/Bin32/
)

# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/src/game/shaders/*.frag"
        "${PROJECT_SOURCE_DIR}/src/game/shaders/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_SOURCE_DIR}/src/game/shaders/${FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
        Shaders
        DEPENDS ${SPIRV_BINARY_FILES}
)