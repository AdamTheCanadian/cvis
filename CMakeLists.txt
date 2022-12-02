cmake_minimum_required(VERSION 3.0)
project(cvis)

set(CMAKE_CXX_STANDARD 14)

find_package(Eigen3 REQUIRED)
find_package(GTest REQUIRED)
add_subdirectory(external)

include_directories(include
        ${EIGEN3_INCLUDE_DIR}
        ${GTEST_INCLUDE_DIR})


include_directories(include)
add_library(${PROJECT_NAME}
        src/camera3d.cpp
        )
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME}
        glfw
        imgui
        glad
        glm)

add_executable(${PROJECT_NAME}_unit_tests
        tests/main.cpp)
target_link_libraries(${PROJECT_NAME}_unit_tests
        ${PROJECT_NAME}
        GTest::GTest GTest::Main)

add_custom_command(
        TARGET ${PROJECT_NAME}_unit_tests
        POST_BUILD
        COMMAND ${PROJECT_NAME}_unit_tests)