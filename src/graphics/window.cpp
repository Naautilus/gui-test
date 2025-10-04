#include "renderer.h"
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include "imgui.h"
#include "implot.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "image/image.h"
#include "../data/data_channel.h"
#include "../data/data_history.h"
#include "../globals/globals.h"

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

void renderer::graph_window(std::string name, data_history data_history_, double min, double max, ImPlotColormap colormap, ImVec2 pos, ImVec2 size) {
    std::lock_guard<std::mutex> lock(globals::history_mutex);    
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(ImVec2(size.x + WINDOW_SIZE_BUFFER * content_scale, size.y + WINDOW_SIZE_BUFFER * content_scale));

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoBackground
                                  | ImGuiWindowFlags_NoFocusOnAppearing
                                  | ImGuiWindowFlags_NoResize
                                  | ImGuiWindowFlags_NoMove
                                  | ImGuiWindowFlags_NoTitleBar
                                  | ImGuiWindowFlags_NoInputs;

    ImGui::Begin(name.c_str(), nullptr, window_flags);

    {
        ImPlot::PushColormap(colormap);

        static ImPlotAxisFlags axis_flags = ImPlotAxisFlags_Lock 
                                          | ImPlotAxisFlags_NoGridLines 
                                          | ImPlotAxisFlags_NoTickMarks 
                                          | ImPlotAxisFlags_NoTickLabels
                                          | ImPlotAxisFlags_NoDecorations
                                          | ImPlotAxisFlags_NoLabel;

        if (ImPlot::BeginPlot("##Heatmap1", ImVec2(size.x - x_size * 0.05, size.y))) {
            ImPlot::SetupAxes(nullptr, nullptr, axis_flags, axis_flags);
            std::vector<double> data_array = data_history_.get_1d_vector();
            ImPlot::PlotHeatmap(name.c_str(), &data_array[0], data_history_.get_width(), data_history_.get_length(), min, max, nullptr, ImPlotPoint(0,0), ImPlotPoint(1,1));
            ImPlot::EndPlot();
        }
        ImGui::SameLine();
        ImPlot::ColormapScale("##ColormapScale", min, max, ImVec2(x_size * 0.05, size.y));
        ImPlot::PopColormap();
    }

    std::vector<ImVec2> label_positions;

    double graph_start_y = pos.y + 2 * GRAPH_BORDER_WIDTH * content_scale;
    double graph_end_y = pos.y + size.y;
    double graph_side_x = pos.x + size.x - x_size * 0.05;

    std::vector<std::string> labels = data_history_.get_labels().get_data();
    for (int i = 0; i < labels.size(); i++) {
        std::string label = labels[i];
        double x = graph_side_x - ImGui::CalcTextSize(label.c_str()).x;
        double y = graph_start_y + (i * 1.0 / labels.size()) * (graph_end_y - graph_start_y);
        y += 0.5 * (graph_end_y - graph_start_y) / labels.size() - 0.5 * ImGui::CalcTextSize(label.c_str()).y;
        std::string label_window_name = name;
        label_window_name += "_";
        label_window_name += i;

        ImGui::SetCursorPos(ImVec2(x - pos.x + content_scale, y - pos.y + content_scale));
        ImGui::TextColored(ImVec4(0.0, 0.0, 0.0, 1.0), label.c_str());
        
        ImGui::SetCursorPos(ImVec2(x - pos.x, y - pos.y));
        ImGui::Text(label.c_str());
    }

    ImGui::End();
}

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

void renderer::automation_window() {
    ImGui::SetNextWindowPos(ImVec2(x_size * 0.005, y_size * 0.385));
    ImGui::SetNextWindowSize(ImVec2(x_size * 0.37, y_size * 0.61));
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

void renderer::communications_window() {
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