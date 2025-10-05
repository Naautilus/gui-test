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
    
void renderer::communications_window() {
    std::lock_guard<std::mutex> lock(globals::globals_mutex);
    ImGui::SetNextWindowPos(ImVec2(x_size * 0.385, y_size * 0.005));
    ImGui::SetNextWindowSize(ImVec2(x_size * 0.61, y_size * 0.37));

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize
                                  | ImGuiWindowFlags_NoMove;

    ImGui::Begin("Communications", nullptr, window_flags);

    ImGui::PushFont(globals::font_deja_vu, 16.0);
    ImGui::Checkbox("TX", &globals::enable_tx);
    ImGui::Checkbox("RX", &globals::enable_rx);

    bool good_rx_time;
    if (!globals::last_rx) good_rx_time = false;
    else good_rx_time = (std::chrono::high_resolution_clock::now() - globals::last_rx.value() < std::chrono::milliseconds(1000));

    ImGui::Text("Connection:");
    ImGui::SameLine();
    {
        std::string status;
        if (!globals::enable_rx) status = "Disabled";
        else if (!globals::last_rx) status = "Waiting for RX...";
        else if (good_rx_time) status = "OK";
        else status = "No RX";
        ImGui::Text(status.c_str());
    }
    ImGui::PopFont();

    ImGui::Text("");
    ImGui::Text("Time since last RX:");
    ImGui::SameLine();
    {
        std::string status;
        if (!globals::enable_rx) status = "-";
        else if (!globals::last_rx) status = "-";
        else status = std::to_string((int)floor(std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - globals::last_rx.value()).count()));
        status += " s";
        ImGui::Text(status.c_str());
    }

    ImGui::Text("Protocol: Serial over USB @ 115200 baud");
    ImGui::Text("Error rate from last 10 s: 0.0%%");

    ImGui::PushFont(globals::font_arial, 11.0);
    ImGui::Text("↑");
    ImGui::PopFont();
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x - ImGui::CalcTextSize(" ").x * 0.5, ImGui::GetCursorPos().y));
    ImGui::Text("%.1f Mbps", 0.0);

    ImGui::PushFont(globals::font_arial, 11.0);
    ImGui::Text("↓");
    ImGui::PopFont();
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x - ImGui::CalcTextSize(" ").x * 0.5, ImGui::GetCursorPos().y));
    ImGui::Text("%.1f Mbps", 0.0);

    ImGui::SetCursorPos(ImVec2(x_size * 0.3, y_size * 0.035));

    ImVec2 console_size(x_size * 0.3, y_size * 0.32);

    static int tick;
    static std::string random_;
    if (globals::enable_tx && globals::enable_rx && tick % 30 == 0) {
        random_ = "";
        for (int i = 0; i < 10; i++) random_ += std::to_string(rand() % 10);
        globals::console_text += "↑heartbeat " + random_ + "\n";
    }
    if (globals::enable_tx && globals::enable_rx && tick % 30 == 5) {
        globals::console_text += "↓heartbeat " + random_ + "\n";
        globals::last_rx = std::chrono::high_resolution_clock::now();
    }
    tick++;

    while (ImGui::CalcTextSize(globals::console_text.c_str()).y > (console_size.y - content_scale)) {
        globals::console_text = globals::console_text.substr(1);
    }
    //char buf[8192];
    //strncpy(buf, globals::console_text.c_str(), sizeof(buf));
    //buf[sizeof(buf) - 1] = '\0';
    //ImGui::InputTextMultiline("", buf, sizeof(buf), console_size, ImGuiInputTextFlags_);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::BeginChild("##Console", console_size, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::Text(globals::console_text.c_str());
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::End();
}

}