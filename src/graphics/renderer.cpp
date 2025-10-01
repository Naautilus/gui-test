#include "renderer.h"
#include <iostream>
#include <string>
#include <chrono>
#include <stdio.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "implot.h"
#include <GLFW/glfw3.h>
#include "image/image.h"
#include <vector>
#include "data/data_channel.h"
#include "data/data_history.h"
#include "../globals.h"

const double WINDOW_SIZE_BUFFER = 50;
int x_size;
int y_size;

void initialize_glfw() {
    if (!glfwInit()) {
        std::cerr << "GLFW failed to initialize\n";
        std::abort();
    }
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void image_window(std::string name, image& image, ImVec2 pos, ImVec2 size) {
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(ImVec2(size.x + WINDOW_SIZE_BUFFER, size.y + WINDOW_SIZE_BUFFER));

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

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(ImVec2(size.x + WINDOW_SIZE_BUFFER, size.y + WINDOW_SIZE_BUFFER));

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoBackground
                                    | ImGuiWindowFlags_NoBringToFrontOnFocus
                                    | ImGuiWindowFlags_NoFocusOnAppearing
                                    | ImGuiWindowFlags_NoResize
                                    | ImGuiWindowFlags_NoMove
                                    | ImGuiWindowFlags_NoTitleBar;

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

    ImGui::End();
}

void start_renderer()
{

    glfwSetErrorCallback(glfw_error_callback);
    if (glfw_is_initialized) initialize_glfw();
    
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only

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
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    std::vector<ImU32> colors = {ImColor(0, 30, 0), ImColor(0, 255, 0)};
    ImPlotColormap colormap_green = ImPlot::AddColormap("green", &colors[0], 2, false);

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

        image_window("rocket window", rocket, ImVec2(x_size * 0.4, y_size * 0.5), ImVec2(x_size * 0.1, y_size * 0.3));

        graph_window("Pressure (MPa)", history_pressure, -1, 1, ImPlotColormap_Viridis, ImVec2(x_size * 0.5, y_size * 0.35), ImVec2(x_size * 0.4, y_size * 0.15));
        graph_window("Temperature (*C)", history_temperature, -1, 1, ImPlotColormap_Hot, ImVec2(x_size * 0.5, y_size * 0.5), ImVec2(x_size * 0.4, y_size * 0.3));
        graph_window("Thrust (kN)", history_thrust, -1, 1, colormap_green, ImVec2(x_size * 0.5, y_size * 0.8), ImVec2(x_size * 0.4, y_size * 0.1));
        graph_window("Vibration (m/s^2)", history_vibration, -1, 1, ImPlotColormap_Pink, ImVec2(x_size * 0.5, y_size * 0.9), ImVec2(x_size * 0.4, y_size * 0.04));

        
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        tick++;
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}
