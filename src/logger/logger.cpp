#include "logger.h"
#include "../globals/globals.h"
#include <ctime>

logger::logger(std::string name, double duration_) {
    time_t now;
    time(&now);
    char timestamp[sizeof("_YYYY-MM-DD_HH-MM-SS")];
    strftime(timestamp, sizeof(timestamp), "_%Y-%m-%d_%H-%M-%S", localtime(&now));

    filename = name;
    filename += timestamp;
    filename += ".tsv";

    std::string filepath = "../logs/" + filename;
    file.open(filepath, std::ios::trunc);

    write(true);

    start_time = std::chrono::high_resolution_clock::now();

    if (duration_ < 0) duration = std::nullopt;
    else duration = std::make_optional(duration_);
}

std::optional<double> logger::get_duration() {
    return duration;
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