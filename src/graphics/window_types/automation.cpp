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

    ImGui::SetNextWindowPos(ImVec2(x_size * 0.005, y_size * 0.485));
    ImGui::SetNextWindowSize(ImVec2(x_size * 0.37, y_size * 0.51));

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize
                                  | ImGuiWindowFlags_NoMove;

    ImGui::Begin("Automation", nullptr, window_flags);

    std::vector<valve> valves_ = globals::valves.get_data();
    
    if (!globals::enable_tx) ImGui::SeparatorText("Valve Actuation");
    else ImGui::SeparatorText("Valve Actuation [WARNING: Changes are live!]");

    for (int i = 0; i < valves_.size(); i++) {
        valve& v = valves_[i];
        int valve_activation_mode;
        if (!v.is_manual()) valve_activation_mode = 1;
        else valve_activation_mode = 0;
        {
            std::string manual_text = "Constant##" + std::to_string(i);
            std::string auto_text = "##" + std::to_string(i);
            ImGui::RadioButton(manual_text.c_str(), &valve_activation_mode, 0);
            ImGui::SameLine();
            ImGui::RadioButton(auto_text.c_str(), &valve_activation_mode, 1);
            ImGui::SameLine();
        }
        bool manual_activation = (valve_activation_mode != 1);
        
        
        
        if (manual_activation) ImGui::BeginDisabled();

        const float TIME_BUFFER = 0.01f;

        ImGui::SetNextItemWidth(ImGui::CalcTextSize("  Open: T + %.3f s  |  Close: T + %.3f s  ").x);
        std::string drag_name = "##" + v.get_name();
        float open_time = (float)v.open_time;
        float close_time = (float)v.close_time;
        ImGui::DragFloatRange2(drag_name.c_str(), &open_time, &close_time, 0.01f, TIME_BUFFER, globals::sequence_max_time - TIME_BUFFER, "Open: T + %.3f s", "Close: T + %.3f s", ImGuiSliderFlags_AlwaysClamp);
        v.open_time = fmin((double)open_time, globals::sequence_max_time - TIME_BUFFER);
        v.close_time = fmin((double)close_time, globals::sequence_max_time - TIME_BUFFER);
        ImGui::SameLine();

        if (manual_activation) ImGui::EndDisabled();
        if (!manual_activation) ImGui::BeginDisabled();

        bool activated = v.is_open(globals::sequence_time, globals::sequence_max_time);
        ImGui::Checkbox(v.get_name().c_str(), &activated);
        if (manual_activation) v.set_to_manual_activation(activated);
        else v.set_to_automatic_activation(); // this isn't necessary to run each frame, but might as well

        if (!manual_activation) ImGui::EndDisabled();
    }
    globals::valves.set_data(valves_);

    ImGui::End();
}

}