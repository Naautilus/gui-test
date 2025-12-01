#pragma once
#include "GLFW/glfw3.h"

namespace graphics {

/*
| backend is a class which hides the complicated
| logic of initializing imgui and OpenGL.
*/

class backend {
    GLFWwindow* window;
    public:
    backend(double& content_scale, int& x_size, int& y_size, bool fullscreen);
    bool start_of_frame();
    void end_of_frame();
    void end();
    bool is_open();
};

}