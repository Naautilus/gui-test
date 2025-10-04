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
#include "../globals.h"

const double WINDOW_SIZE_BUFFER = 25;
const double GRAPH_BORDER_WIDTH = 5;
double content_scale;
int x_size;
int y_size;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void image_window(std::string name, image& image, ImVec2 pos, ImVec2 size) {
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

void graph_window(std::string name, data_history data_history_, double min, double max, ImPlotColormap colormap, ImVec2 pos, ImVec2 size) {
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
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
        /*
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
        */

        {
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

        {
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

        {
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
