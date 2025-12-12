#include "dashboard.hpp"
#include "imgui.h"

namespace ui {
	void render_dashboard() {
		ImGui::Begin("dashboard");
		ImGui::Text("Dashboard placeholder");
		ImGui::End();
	}
}
