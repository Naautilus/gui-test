#pragma once
#include <string>
#include "GLFW/glfw3.h"

/*
| image is a struct which hides the complicated logic of loading an image
| into OpenGL. Load an image by image(filepath) and it will deal with the rest.
*/

struct image {
    int width, height = 0;
    GLuint texture = 0;
    image();
    image(std::string path);
};