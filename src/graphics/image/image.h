#pragma once
#include <string>
#include "GLFW/glfw3.h"

struct image {
    int width, height = 0;
    GLuint texture = 0;
    image();
    image(std::string path);
};