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
    
void renderer::graph_window(std::string name, data_history data_history_, double min, double max, ImPlotColormap colormap, ImVec2 pos, ImVec2 size) {
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

}