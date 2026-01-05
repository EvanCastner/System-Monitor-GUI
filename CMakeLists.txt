cmake_minimum_required(VERSION 3.16)
project(SystemMonitorGUI LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- GLFW platform config ----
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)

# Build GLFW from submodule
add_subdirectory(external/glfw)

# ImGui + ImPlot sources
set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_demo.cpp

    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp

    external/implot/implot.cpp
    external/implot/implot_items.cpp
    external/implot/implot_demo.cpp
)

# Project sources
set(PROJECT_SOURCES
    src/main.cpp

    src/monitor/cpu.cpp
    src/monitor/memory.cpp
    src/monitor/network.cpp

    src/ui/dashboard.cpp
)

# Executable
add_executable(SystemMonitor
    ${PROJECT_SOURCES}
    ${IMGUI_SOURCES}
)

add_custom_target(run
    COMMAND SystemMonitor
    DEPENDS SystemMonitor
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)


# Include directories
target_include_directories(SystemMonitor PRIVATE
    src
    external/imgui
    external/imgui/backends
    external/implot
    external/glfw/include
)

# macOS frameworks
if(APPLE)
    target_link_libraries(SystemMonitor
        glfw
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
else()
    target_link_libraries(SystemMonitor
        glfw
        OpenGL::GL
    )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(SystemMonitor PRIVATE -Wall -Wextra -Wpedantic)
endif()
