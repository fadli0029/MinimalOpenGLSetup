cmake_minimum_required(VERSION 3.12)
project(LearningOpenGL)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
find_package(Eigen3 REQUIRED)

include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)

FetchContent_MakeAvailable(glm)

include_directories(
    include
    include/cameras
    include/input_handlers
    ${EIGEN3_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIRS}
)

add_library(glad src/glad.c)
target_include_directories(glad PUBLIC include)

add_library(foo src/foo.cpp)
target_include_directories(foo PUBLIC include)

add_executable(LearningOpenGL main.cpp)

target_link_libraries(LearningOpenGL
    PRIVATE
    glad
    foo
    SDL2::SDL2
    SDL2::SDL2main
    glm::glm
)

# Set the working directory to the root of the project for the executable
set_target_properties(LearningOpenGL PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/.."
)
