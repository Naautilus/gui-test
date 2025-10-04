#include <iostream>
#include <string>
#include <chrono>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "implot.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "renderer.h"
#include "image/image.h"
#include "../data/data_channel.h"
#include "../data/data_history.h"
#include "../globals/globals.h"

namespace renderer {

double content_scale;
int x_size;
int y_size;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void start_renderer()
{

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        std::cerr << "GLFW failed to initialize\n";
        std::abort();
    }
    
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    content_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Dear ImGui GLFW+OpenGL3 example", glfwGetPrimaryMonitor(), nullptr);

    glfwGetWindowSize(window, &x_size, &y_size);

    if (window == nullptr)
        std::abort();
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    globals::initialize_images();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(content_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = content_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);

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
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        
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
        
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

}