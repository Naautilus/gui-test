#include "../renderer.h"
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include "imgui.h"
#include "implot.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "../image/image.h"
#include "../../data/data_channel.h"
#include "../../data/data_history.h"
#include "../../globals/globals.h"

namespace graphics {
    
void renderer::automation_window() {
    std::lock_guard<std::mutex> lock(globals::globals_mutex);

    ImGui::SetNextWindowPos(ImVec2(x_size * 0.005, y_size * 0.385));
    ImGui::SetNextWindowSize(ImVec2(x_size * 0.37, y_size * 0.61));

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize
                                  | ImGuiWindowFlags_NoMove;

    ImGui::Begin("Automation", nullptr);

    ImGui::SeparatorText("Valve Timings");
    std::vector<valve> valves_ = globals::valves.get_data();
    for (valve& v : valves_) {
        float open_time = (float)v.open_time;
        float close_time = (float)v.close_time;
        ImGui::DragFloatRange2(v.get_name().c_str(), &open_time, &close_time, 0.01f, -1.0f, 30.0f, "Open: T + %.3f s", "Close: T + %.3f s", ImGuiSliderFlags_AlwaysClamp);
        v.open_time = (double)open_time;
        v.close_time = (double)close_time;
    }

    ImGui::SeparatorText("Valve Actuation");
    
    bool manual_activation;
    {
        static int valve_activation_mode = 0;
        ImGui::RadioButton("Manual", &valve_activation_mode, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Automatic", &valve_activation_mode, 1);
        manual_activation = (valve_activation_mode == 0);
    }

    if (!manual_activation) ImGui::BeginDisabled();
    for (valve& v : valves_) {
        bool activated = v.is_open(globals::sequence_time);
        ImGui::Checkbox(v.get_name().c_str(), &activated);
        if (manual_activation) v.set_to_manual_activation(activated);
        else v.set_to_automatic_activation(); // this isn't necessary to run each frame, but might as well
    }
    globals::valves.set_data(valves_);
    if (!manual_activation) ImGui::EndDisabled();

    ImGui::End();
}

}