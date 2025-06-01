#include "overlay.h"

#include "imgui/imgui.h"

#include "iracing.h"
#include <functional>
#include <imgui_internal.h>

struct HUDBox {
    std::string label;
    ImVec4 labelColor = ImVec4(1, 1, 1, 1);
};

ImVec2 MeasureImGuiContentSize(std::function<void()> contentDrawFunc) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImVec2 oldClipMin = window->ClipRect.Min;
    ImVec2 oldClipMax = window->ClipRect.Max;

    // Clip offscreen to hide drawing
    window->ClipRect.Min = ImVec2(-10000, -10000);
    window->ClipRect.Max = ImVec2(-9999, -9999);

    ImGui::BeginGroup();
    contentDrawFunc();
    ImGui::EndGroup();

    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();

    window->ClipRect.Min = oldClipMin;
    window->ClipRect.Max = oldClipMax;

    return ImVec2(max.x - min.x, max.y - min.y);
}

void DrawHUDBox(const HUDBox& box, float boxWidth, float boxHeight, std::function<void()> drawChildren) {
    ImVec2 boxMin = ImGui::GetCursorScreenPos();
    ImVec2 boxMax = ImVec2(boxMin.x + boxWidth, boxMin.y + boxHeight);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRect(boxMin, boxMax, IM_COL32(255, 255, 255, 255), 4.0f, 0, 1.5f);

    ImGui::InvisibleButton("##HUDBox", ImVec2(boxWidth, boxHeight));

    float textY = boxMin.y + 6.0f;

    // Draw label
    if (!box.label.empty()) {
        ImVec2 textSize = ImGui::CalcTextSize(box.label.c_str());
        float textX = boxMin.x + (boxWidth - textSize.x) * 0.5f;
        drawList->AddText(ImVec2(textX, textY), ImGui::ColorConvertFloat4ToU32(box.labelColor), box.label.c_str());
        textY += textSize.y + 4.0f; // spacing after label
    }

    // Measure children size (width and height)
    ImVec2 childrenSize = MeasureImGuiContentSize(drawChildren);

    // Calculate offsets to center children inside remaining box area
    float remainingHeight = boxHeight - (textY - boxMin.y);
    float offsetY = textY + (remainingHeight - childrenSize.y) * 0.5f;

    float offsetX = boxMin.x + (boxWidth - childrenSize.x) * 0.5f;

    // Set cursor to start position for children (screen coords)
    ImGui::SetCursorScreenPos(ImVec2(offsetX, offsetY));

    // Draw children normally (will be left-aligned relative to this cursor pos)
    drawChildren();

    // Advance cursor below the box
    ImGui::SetCursorScreenPos(ImVec2(boxMin.x, boxMax.y));
}



void overlay::draw_ddu() {
    ImGui::SetNextWindowSize(ImVec2(900, 300), ImGuiCond_Always);

    ImGui::Begin("Race HUD", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::BeginGroup();
    HUDBox pos;
    pos.label = "Pos";
    pos.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(pos, 60, 60, []() {
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "37");
        });

    HUDBox lap_delta;
    lap_delta.label = "Lap D";
    lap_delta.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(lap_delta, 60, 60, []() {
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "-5");
        });

    HUDBox inc;
    inc.label = "Inc";
    inc.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(inc, 60, 60, []() {
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "16x");
        });

    ImGui::EndGroup();

    ImGui::SameLine(); ImGui::BeginGroup();

    HUDBox session_time;
    session_time.label = "Session";
    session_time.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(session_time, 120, 60, []() {
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "13:17");
        });

    HUDBox lap;
    session_time.label = "Lap";
    session_time.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(session_time, 120, 80, []() {
        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "23 / 300");
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "273 TO GO");
        ImGui::EndGroup();
        });

    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::BeginGroup();

    HUDBox best_lap;
    best_lap.label = "Best Lap";
    best_lap.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(best_lap, 120, 60, []() {
        ImGui::TextColored(ImVec4(1, 0, 1, 1), "16.003");

        });

    HUDBox last_lap;
    last_lap.label = "Last Lap";
    last_lap.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(last_lap, 120, 60, []() {
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "16.003");

        });

    HUDBox p1_last;
    p1_last.label = "P1 Last";
    p1_last.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(p1_last, 120, 60, []() {
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "16.003");

        });

    ImGui::EndGroup();

    ImGui::End();
}