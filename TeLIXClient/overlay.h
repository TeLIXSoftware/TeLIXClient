#pragma once

#define NOMINMAX

namespace overlay
{
	void init();
	void render();
	void draw_test_gui();


	// overlays
	void draw_ddu();

	void cleanup();
	bool isImGuiContext();
};

