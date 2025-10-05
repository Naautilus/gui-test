#include <thread>
#include "math.h"
#include "graphics/renderer.h"
#include "globals/globals.h"
#include "systems/simulated_data.h"

int main() {
    const double DELTA_T = 0.001;

    globals::globals_mutex.lock();
    add_logger(logger_type::automatic);
    globals::globals_mutex.unlock();

    auto time_interval = std::chrono::microseconds((int)(DELTA_T * 1e6));

    std::thread t(graphics::start_renderer);

    srand(time(NULL));
    simulated_data simulated_data_;

    for (int tick = 0; true; tick++) {
        auto time_start = std::chrono::high_resolution_clock::now();
        
        globals::globals_mutex.lock();
        if (globals::fired) globals::sequence_time += DELTA_T;
        globals::globals_mutex.unlock();
        
        if (tick % 10 == 0) simulated_data_.update();

        globals::globals_mutex.lock();
        for (logger& l : globals::loggers) l.write();
        globals::globals_mutex.unlock();

        if (tick > 3000) while (std::chrono::high_resolution_clock::now() - time_start < time_interval);
    }
}