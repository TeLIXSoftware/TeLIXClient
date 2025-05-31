#include "overlay.h"
#include "imgui/imgui.h"

void overlay::draw_test_gui() {
	ImGui::SetNextWindowSize(ImVec2(600, 400));

	ImGui::Begin("gui", nullptr, ImGuiWindowFlags_NoDecoration);
	{
		ImGui::Text("overlay test");
	}
	ImGui::End();
}