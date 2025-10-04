#include "logger.h"
#include "globals.h"

logger::logger(std::string filepath) {
    file.open(filepath, std::ios::trunc);
}

namespace {

write(std::ofstream& file, bool title, data_history& data_history_) {
    for (int i = 0; i < data_history_.get_width(); i++) {
        if (title) file << data_history_.get_labels().get_data()[i] << "\t";
        else file << data_history_.get_current_data().get_data()[i] << "\t";
    }
}

}

logger::write(bool title) {
    
    file << (title ? "enable_tx"     : (double)globals::enable_tx) << "\t";
    file << (title ? "enable_rx"     : (double)globals::enable_rx) << "\t";
    file << (title ? "fired"         : (double)globals::fired) << "\t";
    file << (title ? "sequence_time" : globals::sequence_time) << "\t";
    {
        std::lock_guard<std::mutex> lock(globals::history_mutex);
        write(file, true, globals::history_temperature);
        write(file, true, globals::history_pressure);
        write(file, true, globals::history_thrust);
        write(file, true, globals::history_vibration);
        write(file, true, globals::history_valves);
    }

}