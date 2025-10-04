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
    ImGui::SetNextWindowPos(ImVec2(x_size * 0.005, y_size * 0.005));
    ImGui::SetNextWindowSize(ImVec2(x_size * 0.37, y_size * 0.37));
    ImGui::Begin("Control", nullptr);
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
    }
    ImGui::End();
}

}