#include "logger.h"
#include "../globals/globals.h"

logger::logger(std::string filepath) {
    file.open(filepath, std::ios::trunc);
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
    file << (title ? "enable_tx"     : std::to_string((double)globals::enable_tx)) << "\t";
    file << (title ? "enable_rx"     : std::to_string((double)globals::enable_rx)) << "\t";
    file << (title ? "fired"         : std::to_string((double)globals::fired)) << "\t";
    file << (title ? "sequence_time" : std::to_string(globals::sequence_time)) << "\t";
    {
        std::lock_guard<std::mutex> lock(globals::history_mutex);
        write_data_history(file, true, globals::history_temperature);
        write_data_history(file, true, globals::history_pressure);
        write_data_history(file, true, globals::history_thrust);
        write_data_history(file, true, globals::history_vibration);
        write_data_history(file, true, globals::history_valves);
    }
}