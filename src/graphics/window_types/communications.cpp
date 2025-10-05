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
    ImGui::Begin("Communications", nullptr);
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
    ImGui::End();
}

}