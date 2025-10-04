#pragma once
#include <fstream>

struct logger {
    std::ofstream file;
    logger(std::string filepath);
    write(bool title);
}