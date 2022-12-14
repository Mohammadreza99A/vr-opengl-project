cmake_minimum_required(VERSION 3.0)
set(PROJECT_NAME vr_opengl_project)
project(${PROJECT_NAME})

set (CMAKE_BUILD_TYPE Release)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-variable -Wno-unused-parameter ")
set (CMAKE_CXX_FLAGS_DEBUG "-g")
set (CMAKE_CXX_FLAGS_RELEASE "-O3")
set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")

include_directories(include)

set(INCLUDE_PATH "/usr/include/glm")
set(LIB_PATH "/usr/lib")

# for glad library
add_subdirectory(vendor/glad/)

include_directories(${INCLUDE_PATH} vendor/stb/)
link_directories(${LIB_PATH})

file(GLOB SOURCE_FILES "src/*.cpp" "include/*.h" "include/*.hpp")
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

#copy all glsl files to build folder
file(GLOB_RECURSE GLSL_FILES "shaders/*.glsl")

foreach(glsl_file ${GLSL_FILES})
    configure_file(${glsl_file} . COPYONLY)
endforeach(glsl_file)

add_compile_definitions(PATH_TO_OBJECTS="${CMAKE_CURRENT_SOURCE_DIR}/objects")
add_compile_definitions(PATH_TO_TEXTURE="${CMAKE_CURRENT_SOURCE_DIR}/textures")
add_compile_definitions(PATH_TO_SHADERS="${CMAKE_CURRENT_SOURCE_DIR}/shaders")

target_link_libraries(${PROJECT_NAME} glad)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} IL)