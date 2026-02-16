#pragma once 

namespace ui {
	/**
	 * Renders the main system monitoring dashboard
	 * 
	 * Creates and displays an ImGui window containing real-time system statistics:
	 * - CPU usage with historical line graph
	 * - Memory usage with prograss bar visualization
	 * - Network upload/download speeds with historical graphs
	 * - Cumulative network transfer totals
	 * 
	 * This function should be called every frame in the main render loop
	 * Uses static data structures internally to maintain monitoring history
	 */
	void render_dashboard();
}
