cmake_minimum_required(VERSION 3.16)
project(SystemMonitorGUI LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Auto-initialize git submodules ----
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    message(STATUS "Checking submodules...")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        RESULT_VARIABLE GIT_SUBMOD_RESULT
    )
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init --recursive failed. Please run it manually.")
    endif()
endif()

# ---- Auto-install Linux dependencies (Debian/Ubuntu) ----
if(UNIX AND NOT APPLE)
    find_program(APT_GET apt-get)
    if(APT_GET)
        message(STATUS "Installing Linux dependencies...")
        execute_process(COMMAND sudo apt-get update)
        execute_process(
            COMMAND sudo apt-get install -y
                libxrandr-dev
                libxinerama-dev
                libxcursor-dev
                libxi-dev
                libxext-dev
                libgl1-mesa-dev
            RESULT_VARIABLE APT_RESULT
        )
        if(NOT APT_RESULT EQUAL "0")
            message(WARNING "apt-get failed. Please manually install: libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxext-dev libgl1-mesa-dev")
        endif()
    endif()
endif()

# ---- GLFW platform config ----
if(APPLE OR WIN32)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
else()
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
endif()

add_subdirectory(external/glfw)

# ---- OpenGL ----
find_package(OpenGL REQUIRED)

# ---- ImGui + ImPlot sources ----
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

# ---- Project sources ----
set(PROJECT_SOURCES
    src/main.cpp
    src/monitor/cpu.cpp
    src/monitor/memory.cpp
    src/monitor/network.cpp
    src/ui/dashboard.cpp
)

# ---- Executable ----
add_executable(SystemMonitor
    ${PROJECT_SOURCES}
    ${IMGUI_SOURCES}
)

# ---- Include directories ----
target_include_directories(SystemMonitor PRIVATE
    src
    external/imgui
    external/imgui/backends
    external/implot
    external/glfw/include
)

# ---- Platform-specific linking ----
if(APPLE)
    target_link_libraries(SystemMonitor
        glfw
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
elseif(WIN32)
    target_link_libraries(SystemMonitor
        glfw
        OpenGL::GL
        gdi32
        user32
        shell32
    )
else()
    target_link_libraries(SystemMonitor
        glfw
        OpenGL::GL
        dl
        pthread
    )
endif()

# ---- Compiler warnings (GCC/Clang only) ----
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(SystemMonitor PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ---- Run target ----
add_custom_target(run
    COMMAND SystemMonitor
    DEPENDS SystemMonitor
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
