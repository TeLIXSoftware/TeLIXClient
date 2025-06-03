#pragma once

#define NOMINMAX

namespace overlay
{
	void init();
	void render();
	void draw_test_gui();


	// overlays
	void draw_ddu();
	void draw_telemetry();

	void cleanup();
	bool isImGuiContext();
};

