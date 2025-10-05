#include "logger.h"
#include "../globals/globals.h"
#include <iostream>
#include <ctime>

logger::logger(logger_type logger_type__, std::optional<double> duration_) {

    logger_type_ = logger_type__;

    time_t now;
    time(&now);
    char timestamp[sizeof("_YYYY-MM-DD_HH-MM-SS")];
    strftime(timestamp, sizeof(timestamp), "_%Y-%m-%d_%H-%M-%S", localtime(&now));

    filename = get_name_for_logger_type();
    filename += "_log_";
    filename += timestamp;
    filename += ".tsv";

    std::string filepath = "../logs/" + filename;
    file.open(filepath, std::ios::trunc);

    write(true);

    start_time = std::chrono::high_resolution_clock::now();

    duration = duration_;
}

void logger::set_duration(std::optional<double> duration_) {
    start_time = std::chrono::high_resolution_clock::now();
    duration = duration_;
}

std::optional<double> logger::get_duration() {
    return duration;
}

std::string logger::get_name_for_logger_type() {
    switch (logger_type_) {
        case logger_type::automatic: return "auto";
        case logger_type::manual: return "manual";
        case logger_type::sequence: return "sequence";
        case logger_type::tx: return "tx";
        case logger_type::rx: return "rx";
        default: return "";
    }
}

namespace {

void write_data_history(std::ofstream& file, bool title, data_history& data_history_) {
    for (int i = 0; i < data_history_.get_width(); i++) {
        if (title) file << data_history_.get_labels().get_data()[i] << "\t";
        else file << data_history_.get_current_data().get_data()[i] << "\t";
    }
}

}

void logger::write(bool title) {

    calculate_bitrate();

    if (duration && time_since_creation() > duration.value()) return;

    file << (title ? "logger_time"   : std::to_string(time_since_creation())) << "\t";
    file << (title ? "enable_tx"     : std::to_string((double)globals::enable_tx)) << "\t";
    file << (title ? "enable_rx"     : std::to_string((double)globals::enable_rx)) << "\t";
    file << (title ? "fired"         : std::to_string((double)globals::fired)) << "\t";
    file << (title ? "sequence_time" : std::to_string(globals::sequence_time)) << "\t";
    {
        write_data_history(file, title, globals::history_temperature);
        write_data_history(file, title, globals::history_pressure);
        write_data_history(file, title, globals::history_thrust);
        write_data_history(file, title, globals::history_vibration);
        write_data_history(file, title, globals::history_valves);
    }
    file << "\n";
}

double logger::time_since_creation() {
    return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start_time).count();
}

std::string logger::get_filename() {
    return filename;
}

logger_type logger::get_logger_type() {
    return logger_type_;
}

void logger::calculate_bitrate() {

    double time_difference = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - last_bitrate_calculate_time).count();
    if (time_difference < std::chrono::duration<double>(bitrate_calculate_interval).count()) return;
    last_bitrate_calculate_time = std::chrono::high_resolution_clock::now();

    std::streamoff current_file_size = file.tellp();
    auto file_size_difference = current_file_size - last_file_size;
    last_file_size = current_file_size;

    bits_per_second = 8 * file_size_difference / time_difference;

}

double logger::get_bitrate() {
    return bits_per_second;
}

void add_logger(logger_type logger_type_, std::optional<double> duration) {
    std::optional<logger*> existing_logger_of_type = std::nullopt;
    for (logger& l : globals::loggers) {
        if (l.get_logger_type() != logger_type_) continue;
        existing_logger_of_type = std::make_optional(&l);
    }

    if (existing_logger_of_type) {
        std::cout << "setting duration of type " << existing_logger_of_type.value()->get_filename() << "\n";
        existing_logger_of_type.value()->set_duration(duration);
    }
    else globals::loggers.push_back(logger(logger_type_, duration));
}