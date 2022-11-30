cmake_minimum_required(VERSION 3.0)
project(cvis)

find_package(Eigen3 REQUIRED)
find_package(GTest REQUIRED)
add_subdirectory(external)

include_directories(include)
add_library(${PROJECT_NAME}
        src/camera3d.c
        src/shader.c
        src/window.c
        src/projection.c
        src/grid.c
        src/robot.c
        src/vis.c
        src/waypoints.c
        )
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME}
        glfw
        imgui
        glad
        glm)

add_executable(${PROJECT_NAME}_unit_tests
        tests/main.c)
target_link_libraries(${PROJECT_NAME}_unit_tests
        ${PROJECT_NAME}
        ctest)

add_custom_command(
        TARGET ${PROJECT_NAME}_unit_tests
        POST_BUILD
        COMMAND ${PROJECT_NAME}_unit_tests)