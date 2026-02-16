#pragma once

namespace ui {
	/**
	 * Applies a custom visual theme to the ImGui interface
	 * 
	 * Configure ImGui style settings including colors, spacing, rounding,
	 * and other visual properties to create a consistent look and feel
	 * for the system monitoring dashboard
	 * 
	 * This function should be called once during the application initialization,
	 * before rendering any ImGui windows
	 */
	void apply_theme();
}
