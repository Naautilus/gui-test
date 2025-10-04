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
    
void renderer::image_window(std::string name, image& image, ImVec2 pos, ImVec2 size) {
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(ImVec2(size.x + WINDOW_SIZE_BUFFER * content_scale, size.y + WINDOW_SIZE_BUFFER * content_scale));

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoBackground
                                  | ImGuiWindowFlags_NoBringToFrontOnFocus
                                  | ImGuiWindowFlags_NoFocusOnAppearing
                                  | ImGuiWindowFlags_NoResize
                                  | ImGuiWindowFlags_NoMove
                                  | ImGuiWindowFlags_NoTitleBar;

    ImGui::Begin(name.c_str(), nullptr, window_flags);
    ImGui::Image((ImTextureID)(intptr_t)image.texture, size);
    ImGui::End();
}

}