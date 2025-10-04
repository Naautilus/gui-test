#pragma once
#include <fstream>

struct logger {
    std::ofstream file;
    logger(std::string filepath);
    void write(bool title);
};