include_directories(
    ./
    backends/
    /opt/homebrew/include/
)

set(SRCS
    ./imgui_demo.cpp
    ./imgui_draw.cpp
    ./imgui.cpp
    ./imgui_tables.cpp
    ./imgui_widgets.cpp
    ./backends/imgui_impl_opengl3.cpp
    ./backends/imgui_impl_glfw.cpp
)


add_library(IMGUI ${SRCS})
