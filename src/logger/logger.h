#pragma once
#include <fstream>
#include <chrono>
#include <optional>

class logger {

    std::chrono::high_resolution_clock::duration bitrate_calculate_interval = std::chrono::milliseconds(100);
    std::chrono::high_resolution_clock::time_point last_bitrate_calculate_time = std::chrono::high_resolution_clock::now();
    std::streamoff last_file_size = 0;
    double bits_per_second = 0;
    void calculate_bitrate();

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
    double get_bitrate();
};