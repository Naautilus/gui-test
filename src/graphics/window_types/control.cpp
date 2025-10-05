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
    
void renderer::control_window() {
    std::lock_guard<std::mutex> lock(globals::globals_mutex);
    ImGui::SetNextWindowPos(ImVec2(x_size * 0.005, y_size * 0.005));
    ImGui::SetNextWindowSize(ImVec2(x_size * 0.37, y_size * 0.37));
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize
                                  | ImGuiWindowFlags_NoMove;

    ImGui::Begin("Control", nullptr, window_flags);
    ImGui::SeparatorText("Sequence Control");
    ImGui::Checkbox("Safety", &globals::sequence_control_safety);
    if (globals::sequence_control_safety) ImGui::BeginDisabled();
    std::string fire_text;
    fire_text += "                      \n";
    fire_text += "      ----------      \n";
    fire_text += "      |  FIRE  |      \n";
    fire_text += "      ----------      \n";
    fire_text += "                      \n";
    if (ImGui::Button(fire_text.c_str())) {
        std::cout << "fired\n";
        globals::fired = true;
        globals::last_rx = std::chrono::high_resolution_clock::now();
        globals::loggers.push_back(logger("sequence_log", globals::sequence_max_time));
    }
    if (globals::sequence_control_safety) ImGui::EndDisabled();
    {
        float sequence_max_time_ = globals::sequence_max_time;
        ImGui::SetNextItemWidth(ImGui::CalcTextSize(" 120.0 s ").x);
        ImGui::DragFloat("Sequence Time Limit", &sequence_max_time_, 0.1f, 2.0f, 120.0f, "%.1f s");
        globals::sequence_max_time = sequence_max_time_;
    }

    ImGui::SeparatorText("Loggers");

    double bitrate_sum;
    for (logger& l : globals::loggers) bitrate_sum += l.get_bitrate();
    bitrate_sum /= 1e6;

    ImGui::PushFont(globals::font_arial, 11.0);
    ImGui::Text("↓");
    ImGui::PopFont();
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x - ImGui::CalcTextSize(" ").x * 0.5, ImGui::GetCursorPos().y));
    ImGui::Text("%.1f Mbps", (float)bitrate_sum);

    {
        bool allow_more_loggers = (globals::loggers.size() < 5);
        if (!allow_more_loggers) ImGui::BeginDisabled();
        if (ImGui::Button("+")) {
            globals::loggers.push_back(logger("manual_log"));
        }
        if (!allow_more_loggers) ImGui::EndDisabled();
    }


    for (int i = globals::loggers.size() - 1; i >= 0; i--) {
        logger& l = globals::loggers[i];
        if (l.get_duration() && l.time_since_creation() >= l.get_duration().value()) globals::loggers.erase(globals::loggers.begin() + i);
    }
    for (int i = 0; i < globals::loggers.size(); i++) {
        logger& l = globals::loggers[i];
        ImGui::Text("(o)");
        ImGui::SameLine();
        ImGui::Text(l.get_filename().c_str());
        ImGui::SameLine();
        if (l.get_duration()) {
            std::string duration_text = "(";
            duration_text += std::to_string((int)ceil(l.get_duration().value() - l.time_since_creation()));
            duration_text += ")";
            ImGui::Text(duration_text.c_str());
            ImGui::SameLine();
        }
        ImGui::PushFont(globals::font_arial, 11.0);
        ImGui::Text("↓");
        ImGui::PopFont();
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x - ImGui::CalcTextSize(" ").x * 0.5, ImGui::GetCursorPos().y));
        ImGui::Text("%.1f mbps", (float)(l.get_bitrate() / 1e6));
        ImGui::SameLine();
        {
            ImGui::PushFont(globals::font_deja_vu, 9.0);
            std::string button_text = "X##" + std::to_string(i);
            if (ImGui::Button(button_text.c_str())) {
                globals::loggers.erase(globals::loggers.begin() + i);
                ImGui::PopFont();
                break;
            }
            ImGui::PopFont();
        }
    }
    ImGui::End();
}

}