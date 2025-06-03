#include "overlay.h"

#include "imgui/imgui.h"

#include "iracing.h"
#include <functional>
#include "imgui/imgui_internal.h"

#include <algorithm>

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

void DrawRPMBar(float rpmFraction, float width = 400.0f, float height = 20.0f) {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size(width, height);
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Background
    drawList->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), IM_COL32(50, 50, 50, 255), 4.0f);

    // Colors by threshold (can customize!)
    ImU32 barColor;
    if (rpmFraction < 0.6f)
        barColor = IM_COL32(0, 255, 0, 255);  // Green
    else if (rpmFraction < 0.85f)
        barColor = IM_COL32(255, 255, 0, 255); // Yellow
    else
        barColor = IM_COL32(255, 0, 0, 255); // Red

    // Filled bar
    float fill = width * ImClamp(rpmFraction, 0.0f, 1.0f);
    drawList->AddRectFilled(pos, ImVec2(pos.x + fill, pos.y + height), barColor, 4.0f);

    // Reserve space in layout
    ImGui::Dummy(size);
}



void overlay::draw_ddu() {

    /**/


    int p1carIdx = -1;
    for (int i = 0; i < IR_MAX_CARS; ++i)
    {
        if (ir_getPosition(i) == 1) {
            p1carIdx = i;
            break;
        }
    }

    const int  carIdx = ir_session.driverCarIdx;
    const bool   sessionIsTimeLimited = ir_SessionLapsTotal.getInt() == 32767 && ir_SessionTimeRemain.getDouble() < 48.0 * 3600.0;  // most robust way I could find to figure out whether this is a time-limited session (info in session string is often misleading)
    const double remainingSessionTime = sessionIsTimeLimited ? ir_SessionTimeRemain.getDouble() : -1;
    const int    remainingLaps = sessionIsTimeLimited ? int(0.5 + remainingSessionTime / ir_estimateLaptime()) : (ir_SessionLapsRemainEx.getInt() != 32767 ? ir_SessionLapsRemainEx.getInt() : -1);

    const double sessionTime = remainingSessionTime >= 0 ? remainingSessionTime : ir_SessionTime.getDouble();

    const int    hours = int(sessionTime / 3600.0);
    const int    mins = int(sessionTime / 60.0) % 60;
    const int    secs = (int)fmod(sessionTime, 60.0);

    // inc count
    const int incCount = ir_PlayerCarMyIncidentCount.getInt();

    // RPM
    const float lo = (ir_session.rpmIdle + ir_session.rpmSLFirst) / 2;
    const float hi = ir_session.rpmRedline;
    const float rpm = ir_RPM.getFloat();
    const float rpmPct = (rpm - lo) / (hi - lo);

    // Speed
    const float speedMps = ir_Speed.getFloat();
    const float speedKph = speedMps * 2.23694f;

    // Gear
    const int gear = ir_Gear.getInt();
    char gearC = ' ';
    if (gear == -1)
        gearC = 'R';
    else if (gear == 0)
        gearC = 'N';
    else
        gearC = char(gear + 48);

    // Laps
    const int totalLaps = ir_SessionLapsTotal.getInt();
    const int curLap = ir_Lap.getInt();
    // Position
    const int position = ir_getPosition(ir_session.driverCarIdx);

    // Lap Delta
    const int lapDelta = (p1carIdx != -1) ? ir_getLapDeltaToLeader(ir_session.driverCarIdx, p1carIdx) : 0;

    // Best Time 
    const float bestTime = ir_CarIdxBestLapTime.getFloat(carIdx);

    // Last Lap Time
    const float last_lap_time = ir_LapLastLapTime.getFloat();

    // p1's last time
    const float p1_last_time = (p1carIdx != -1)?ir_CarIdxLastLapTime.getFloat(p1carIdx):0.00;

    

    // fuel
    const float fuelPct = ir_FuelLevelPct.getFloat();
    const float remainingFuel = ir_FuelLevel.getFloat();

    // tires
    const float lf = 100.0f * ir_LFwearM.getFloat();
    const float rf = 100.0f * ir_RFwearM.getFloat();
    const float lr = 100.0f * ir_LRwearM.getFloat();
    const float rr = 100.0f * ir_RRwearM.getFloat();

    // bias
    const float brake_bias = ir_dcBrakeBias.getFloat();

    // temps
    float oilTemp = ir_OilTemp.getFloat();
    float waterTemp = ir_WaterTemp.getFloat();

    /**/


    // positioning 

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 screenSize = io.DisplaySize;

    float x = (screenSize.x) * 0.25f; // Center horizontally
    float y = screenSize.y * .65f; // Snap to bottom
    ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);


    ImGui::SetNextWindowSize(ImVec2(0, 0)); // Let ImGui calculate based on contents
    ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX));

    ImGui::Begin("TeLIX DDU", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_AlwaysAutoResize);

    DrawRPMBar(rpmPct, (775));

    ImGui::BeginGroup();
    HUDBox pos;
    pos.label = "Pos";
    pos.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(pos, 60, 60, [position]() {
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%d", position);
        });

    HUDBox lap_delta;
    lap_delta.label = "Lap D";
    lap_delta.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(lap_delta, 60, 60, [lapDelta]() {
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%d", lapDelta);
        });

    HUDBox inc;
    inc.label = "Inc";
    inc.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(inc, 60, 60, [incCount]() {
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%dx", incCount);
        });

    ImGui::EndGroup();

    ImGui::SameLine(); ImGui::BeginGroup();

    HUDBox session_time;
    session_time.label = "Session";
    session_time.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(session_time, 120, 60, [hours, mins, secs]() {
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%d:%02d:%02d", hours, mins, secs);
        });

    HUDBox lap;
    lap.label = "Lap";
    lap.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(lap, 120, 80, [totalLaps, curLap]() {
        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%d / %d", curLap, totalLaps);
        ImGui::TextColored(ImVec4(0.3, 1, 1, 1), "%d TO GO", (totalLaps - curLap));
        ImGui::EndGroup();
        });

    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::BeginGroup();

    HUDBox best_lap;
    best_lap.label = "Best Lap";
    best_lap.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(best_lap, 120, 60, [bestTime]() {
        ImGui::TextColored(ImVec4(1, 0, 1, 1), "%.2f", bestTime);

        });

    HUDBox last_lap;
    last_lap.label = "Last Lap";
    last_lap.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(last_lap, 120, 60, [last_lap_time]() {
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%.2f", last_lap_time);

        });

    HUDBox p1_last;
    p1_last.label = "P1 Last";
    p1_last.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(p1_last, 120, 60, [p1_last_time]() {
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%.2f", p1_last_time);

        });

    ImGui::EndGroup();

    ImGui::SameLine(); ImGui::BeginGroup();

    HUDBox gear_speed;
    gear_speed.label = "Gear";
    gear_speed.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(gear_speed, 120, 100, [gearC, speedKph]() {
        ImGui::BeginGroup();
        float old_size = ImGui::GetFont()->Scale;
        ImGui::GetFont()->Scale *= 2;
        ImGui::PushFont(ImGui::GetFont());
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%c", gearC);
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%.0f", speedKph);
        ImGui::GetFont()->Scale = old_size;
        ImGui::PopFont();
        ImGui::EndGroup();
        });

    HUDBox vs_best;
    vs_best.label = "vs Best";
    vs_best.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(vs_best, 120, 60, []() {
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "---");

        });

    ImGui::EndGroup();

    // fuel
    ImGui::SameLine(); ImGui::BeginGroup();

    HUDBox fuel;
    fuel.label = "Fuel";
    fuel.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(fuel, 120, 60, [fuelPct]() {
        ImGui::ProgressBar(fuelPct, ImVec2(100, 12));
        });

    HUDBox fuelStats;
    fuelStats.label = "Fuel Stats";
    fuelStats.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(fuelStats, 120, 120, [remainingFuel]() {
        
        ImGui::BeginGroup();

        ImGui::TextColored(ImVec4(1, 1, 1, 1), "Laps");
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "Rem");
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "Per");
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "Fin+");
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "Add");

        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "---");
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%.1f gl", remainingFuel);
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "--- gl");
        ImGui::TextColored(ImVec4(1, 0, 1, 1), "--- gl");
        ImGui::TextColored(ImVec4(0.3, 0.3, 1, 1), "--- gl");
        ImGui::EndGroup();


        });

    ImGui::EndGroup();

    // Tires
    ImGui::SameLine(); ImGui::BeginGroup();

    HUDBox tires;
    tires.label = "Tires";
    tires.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(tires, 120, 60, [lf, lr, rf, rr]() {
        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%.1f", lf);
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%.1f", lr);
        ImGui::EndGroup();

        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%.1f", rf);
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%.1f", rr);
        ImGui::EndGroup();

        });

    ImGui::EndGroup();

    ImGui::SameLine(); ImGui::BeginGroup();

    HUDBox oil;
    oil.label = "Oil";
    oil.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(oil, 60, 60, [oilTemp]() {
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%.1f", oilTemp);
        });

    HUDBox water;
    water.label = "Water";
    water.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(water, 60, 60, [waterTemp]() {
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%.1f", waterTemp);
        });

    HUDBox bias;
    bias.label = "Bias";
    bias.labelColor = ImVec4(1, 1, 0, 1);
    DrawHUDBox(bias, 60, 60, [brake_bias]() {
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s%.1f", brake_bias>0?"+":"-", brake_bias);
        });

    ImGui::EndGroup();

    ImGui::End();
}