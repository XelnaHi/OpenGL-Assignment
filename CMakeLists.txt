cmake_minimum_required(VERSION 3.24)

project(ShaderCourseStarter
        VERSION 0.1.0
        DESCRIPTION "Minimal OpenGL/GLSL starter for 3D and Shader Programming"
        LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# --- Assimp ---
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)          # avoid needing system zlib
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)           # we only need importing, not exporting
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE) # Assimp's own -Wall build can be noisy

FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.4.3
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(assimp)

# Keep third-party projects small and quiet.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# Pinned versions make the course setup reproducible.
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glfw)

add_library(glad STATIC
        external/glad/src/gl.c
)

target_include_directories(glad
        PUBLIC
        external/glad/include
)

add_executable(shader_course
        src/main.cpp
        src/Shader.cpp
        src/Shader.h
        src/Camera.h
)

add_library(stb STATIC
        vendors/stb/stb_image_impl.cpp
)
target_include_directories(stb PUBLIC
        vendors
)

target_link_libraries(shader_course
        PRIVATE
        glfw
        glad
        stb
        glm::glm
        assimp
)

if (MSVC)
    target_compile_options(shader_course PRIVATE /W4 /permissive-)
else ()
    target_compile_options(shader_course PRIVATE -Wall -Wextra -Wpedantic)
endif ()

# Put the executable in a predictable location for single- and multi-config generators.
set_target_properties(shader_course PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
foreach (CONFIG Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${CONFIG}" CONFIG_UPPER)
    set_target_properties(shader_course PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_BINARY_DIR}/bin"
    )
endforeach ()

# --- GLM ---
set(GLM_BUILD_LIBRARY ON CACHE BOOL "" FORCE)
add_subdirectory(vendors/glm)

# Keep runtime shader files synchronized whenever shader_course is built.
add_custom_target(copy_shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders"
        "${CMAKE_BINARY_DIR}/bin/shaders"
        COMMENT "Copying shaders next to executable"
        VERBATIM
)
add_custom_target(copy_resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/res"
        "${CMAKE_BINARY_DIR}/bin/res"
        COMMENT "Copying resources next to executable"
        VERBATIM
)

add_dependencies(shader_course copy_resources)

add_dependencies(shader_course copy_shaders)

# When launching from a generated Visual Studio project, run from the
# executable directory so relative paths such as shaders/basic.vert work.
set_target_properties(shader_course PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Helpful when opening the generated solution in Visual Studio.
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT shader_course)
