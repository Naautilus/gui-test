#pragma once
#include <fstream>
#include <chrono>

class logger {
    std::chrono::high_resolution_clock::time_point start_time;
    std::string filename;
    std::ofstream file;
    public:
    logger(std::string name);
    void write(bool title = false);
    std::string get_filename();
};