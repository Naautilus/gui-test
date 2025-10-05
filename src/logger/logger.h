#pragma once
#include <fstream>
#include <chrono>
#include <optional>

class logger {
    std::chrono::high_resolution_clock::time_point start_time;
    std::string filename;
    std::ofstream file;
    std::optional<double> duration;
    public:
    logger(std::string name, double duration_ = -1);
    std::optional<double> get_duration();
    void write(bool title = false);
    double time_since_creation();
    std::string get_filename();
};