#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include "../renderer.h"
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
    ImGui::Text(globals::serial_communications_state.c_str());
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

    ImVec2 console_tx_size(x_size * 0.15, y_size * 0.32);
    ImVec2 console_rx_size(x_size * 0.15, y_size * 0.32);

    int CONSOLE_TEXT_SIZE_HARD_LIMIT = 100000;

    if (globals::console_tx_text.size() > CONSOLE_TEXT_SIZE_HARD_LIMIT) {
        globals::console_tx_text = globals::console_tx_text.substr(globals::console_tx_text.size() - CONSOLE_TEXT_SIZE_HARD_LIMIT);
    }
    if (globals::console_rx_text.size() > CONSOLE_TEXT_SIZE_HARD_LIMIT) {
        globals::console_rx_text = globals::console_rx_text.substr(globals::console_rx_text.size() - CONSOLE_TEXT_SIZE_HARD_LIMIT);
    }

    while (ImGui::CalcTextSize(globals::console_tx_text.c_str()).y > (console_tx_size.y - content_scale)) {
        globals::console_tx_text = globals::console_tx_text.substr(1);
    }
    while (ImGui::CalcTextSize(globals::console_rx_text.c_str()).y > (console_rx_size.y - content_scale)) {
        globals::console_rx_text = globals::console_rx_text.substr(1);
    }

    for (int i = globals::console_tx_text.size() - 1; i >= 0; i--) {
        if (i >= globals::console_tx_text.size() || globals::console_tx_text[i] != '\n') continue;
        if (globals::console_tx_text.substr(i+1, 3) == "↑") continue;
        globals::console_tx_text.insert(i+1, "↑ ");
    }
    for (int i = globals::console_rx_text.size() - 1; i >= 0; i--) {
        if (i >= globals::console_rx_text.size() || globals::console_rx_text[i] != '\n') continue;
        if (globals::console_rx_text.substr(i+1, 3) == "↓") continue;
        globals::console_rx_text.insert(i+1, "↓ ");
    }

    ImGui::SetCursorPos(ImVec2(x_size * 0.3, y_size * 0.035));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 1.0f, 0.9f, 1.0f));
    ImGui::BeginChild("##Console_TX", console_tx_size, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::Text(globals::console_tx_text.c_str());
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();

    ImGui::SetCursorPos(ImVec2(x_size * 0.45, y_size * 0.035));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.9f, 1.0f, 1.0f));
    ImGui::BeginChild("##Console_RX", console_rx_size, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::Text(globals::console_rx_text.c_str());
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();


    ImGui::End();
}

}