#pragma once
#include <GLFW/glfw3.h>

namespace graphics {

class backend {
    GLFWwindow* window;
    public:
    backend(double& content_scale, int& x_size, int& y_size);
    bool start_of_frame();
    void end_of_frame();
    void end();
    bool is_open();
};

}