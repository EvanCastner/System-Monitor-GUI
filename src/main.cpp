// Supports OpenGL deprecation warnings on macOS
#define GL_SILENCE_DEPRECATION

#include <cstdio>
#include <GLFW/glfw3.h> // GLFW for window creation and OpenGL context management

#include "imgui.h"				// Dear ImGui core
#include "imgui_impl_glfw.h"	// ImGui GLFW backend
#include "imgui_impl_opengl3.h" // ImGui OpenGL3 rendering backend
#include "implot.h"				// ImPlot for plotting graphs

#include "ui/dashboard.hpp" // System monitoring dashboard UI

int main()
{
	// ==== GLFW INTITIALIZATION ====
	// Initialize GLFW library for window management
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	// Set OpenGL version to 3.3 (Core Profile)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// macOS requires forward compatibility to flag for OpenGL 3.3+
#ifdef _APPLE_
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Creates application window
	GLFWwindow *window = glfwCreateWindow(1000, 800, "System Moniter (Test)", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Make the OpenGL context current on this thread
	glfwMakeContextCurrent(window);
	// Enable vsynce (swap interval of 1 = synce to monitor refresh rate)
	glfwSwapInterval(1);

	// ==== IMGUI INTITIALIZATION ====
	// Verify ImGui version compatibility
	IMGUI_CHECKVERSION();

	// Create ImGui context
	ImGui::CreateContext();

	// Apply custom dark color scheme
	ImGui::StyleColorsDark();
	auto &style = ImGui::GetStyle();
	style.WindowRounding = 10.0f;
	style.FrameRounding = 6.0f;

	// Create ImPlot context for graph plotting
	ImPlot::CreateContext();

	// Initialize ImGui backends for GLFW and OpenGL3
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// ==== MAIN RENDER LOOP ====
	while (!glfwWindowShouldClose(window))
	{
		// Process window events
		glfwPollEvents();

		// Start new ImGui frame
		ImGui_ImplOpenGL3_NewFrame(); // OpenGL backend new frame
		ImGui_ImplGlfw_NewFrame();	  // GLFW backend new frame
		ImGui::NewFrame();			  // ImGui core new frame

		// Render the system monitoring dashboard
		ui::render_dashboard();

		// Finalize ImGui rendering for this frame
		ImGui::Render();

		// Get current framebuffer size
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);

		// Set OpenGL viewport to match framebuffer size
		glViewport(0, 0, display_w, display_h);

		// Clear the screen with background color
		glClear(GL_COLOR_BUFFER_BIT);

		// Render ImGui draw data to the screen
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap front and back buffers (display the rendered frame)
		glfwSwapBuffers(window);
	}

	// ==== CLEANUP ====
	// Shutdown ImGui backends
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	// Destroy ImPlot and ImGui contexts
	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	// Destory GLFW window and terminate library
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
