add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(glm)

include_directories(imgui/backends)
include_directories(imgui)
add_library(imgui
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui.cpp)
target_link_libraries(imgui glfw)
target_include_directories(imgui PUBLIC imgui)