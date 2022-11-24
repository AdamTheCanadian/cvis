add_subdirectory(glad)
add_subdirectory(glfw)


# Build cImgui
# Example that I used to get this is 
# https://github.com/cimgui/cimgui/blob/docking_inter/backend_test/example_sdl_opengl3/CMakeLists.txt
set (CMAKE_CXX_STANDARD 11)
add_compile_definitions("CIMGUI_USE_GLFW")
add_compile_definitions("CIMGUI_USE_OPENGL3")
add_definitions("-DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1")
include_directories(cimgui)
include_directories(cimgui/imgui/)

set(IMGUI_SOURCES 
  cimgui/cimgui.cpp 
  cimgui/imgui/imgui.cpp 
  cimgui/imgui/imgui_draw.cpp 
  cimgui/imgui/imgui_demo.cpp 
  cimgui/imgui/imgui_widgets.cpp
  cimgui/imgui/imgui_tables.cpp
)

if (WIN32)
    add_definitions("-DIMGUI_IMPL_API=extern \"C\" __declspec\(dllexport\)")
else(WIN32)
    add_definitions("-DIMGUI_IMPL_API=extern \"C\" ")
endif(WIN32)

list(APPEND IMGUI_SOURCES cimgui/imgui/backends/imgui_impl_glfw.cpp)
list(APPEND IMGUI_SOURCES cimgui/imgui/backends/imgui_impl_opengl3.cpp)
include_directories(cimgui/imgui/backends)
add_library(cimgui ${IMGUI_SOURCES})
target_compile_definitions(cimgui PUBLIC -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_GLFW)
target_link_libraries(cimgui glfw)
target_include_directories(cimgui PUBLIC cimgui/)
target_include_directories(cimgui PUBLIC cimgui/generator/output)