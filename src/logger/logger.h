#pragma once
#include <fstream>
#include <chrono>
#include <optional>

enum logger_type {
    automatic,
    manual,
    sequence,
    tx,
    rx
};

class logger {

    std::chrono::high_resolution_clock::duration bitrate_calculate_interval = std::chrono::milliseconds(100);
    std::chrono::high_resolution_clock::time_point last_bitrate_calculate_time = std::chrono::high_resolution_clock::now();
    std::streamoff last_file_size = 0;
    double bits_per_second = 0;
    void calculate_bitrate();

    logger_type logger_type_;
    std::string get_name_for_logger_type();
    std::chrono::high_resolution_clock::time_point start_time;
    std::string filename;
    std::ofstream file;
    std::optional<double> duration;

    public:

    logger(logger_type logger_type__, std::optional<double> duration_ = std::nullopt);
    void set_duration(std::optional<double> duration_);
    std::optional<double> get_duration();
    void write(bool title = false);
    double time_since_creation();
    std::string get_filename();
    double get_bitrate();
    logger_type get_logger_type();
};

void add_logger(logger_type logger_type_, std::optional<double> duration = std::nullopt);