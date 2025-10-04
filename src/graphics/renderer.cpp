#include <iostream>
#include <string>
#include <chrono>
#include <stdio.h>
#include <vector>
#include <math.h>
#include "imgui.h"
#include "implot.h"
#include "renderer.h"
#include "backend.h"
#include "image/image.h"
#include "../data/data_channel.h"
#include "../data/data_history.h"
#include "../globals/globals.h"

namespace graphics {

void start_renderer() {
    renderer renderer_;
}

double content_scale;
int x_size;
int y_size;

renderer::renderer() {

    backend backend_(content_scale, x_size, y_size);
    
    // Our state
    globals::initialize_images();
    bool show_demo_window = true;
    bool show_another_window = false;

    ImPlotColormap colormap_green;
    ImPlotColormap colormap_pink;
    ImPlotColormap colormap_blue;
    {
        std::vector<ImU32> colors = {ImColor(0, 30, 0), ImColor(0, 255, 0), ImColor(200, 255, 200)};
        colormap_green = ImPlot::AddColormap("green", &colors[0], colors.size(), false);
    }
    {
        std::vector<ImU32> colors = {ImColor(30, 0, 30), ImColor(255, 0, 255), ImColor(255, 200, 255)};
        colormap_pink = ImPlot::AddColormap("pink", &colors[0], colors.size(), false);
    }
    {
        std::vector<ImU32> colors = {ImColor(0, 15, 30), ImColor(0, 127, 255)};
        colormap_blue = ImPlot::AddColormap("blue", &colors[0], colors.size(), false);
    }

    // Main loop
    while (backend_.is_open())
    {   
        if (!backend_.start_of_frame()) continue;

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

        control_window();
        automation_window();
        communications_window();

        image_window("rocket window", globals::rocket, ImVec2(x_size * 0.475, y_size * 0.65), ImVec2(x_size * 0.1, y_size * 0.3));

        graph_window("Valve Activations", globals::history_valves, 0.0, 1.0, colormap_blue, ImVec2(x_size * 0.635, y_size * 0.375), ImVec2(x_size * 0.35, y_size * 0.15));
        graph_window("Pressure (mmHg)", globals::history_pressure, 0.0, 1000.0, ImPlotColormap_Viridis, ImVec2(x_size * 0.635, y_size * 0.525), ImVec2(x_size * 0.35, y_size * 0.15));
        graph_window("Temperature (Rankine)", globals::history_temperature, 0.0, 500.0, ImPlotColormap_Hot, ImVec2(x_size * 0.635, y_size * 0.675), ImVec2(x_size * 0.35, y_size * 0.15));
        graph_window("Thrust (stones)", globals::history_thrust, 0.0, 10.0, colormap_green, ImVec2(x_size * 0.635, y_size * 0.825), ImVec2(x_size * 0.35, y_size * 0.08));
        graph_window("Vibration (ft/min^2)", globals::history_vibration, 0.0, 100.0, colormap_pink, ImVec2(x_size * 0.635, y_size * 0.905), ImVec2(x_size * 0.35, y_size * 0.08));
        
        backend_.end_of_frame();
    }
    backend_.end();
}

}