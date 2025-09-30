#pragma once
#include <GLFW/glfw3.h>
#include <string>

struct image {
    int width, height = 0;
    GLuint texture = 0;
    image(std::string path);
};