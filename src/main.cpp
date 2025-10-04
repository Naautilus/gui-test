#include <thread>
#include "math.h"
#include "graphics/renderer.h"
#include "globals/globals.h"
#include "systems/simulated_data.h"

int main() {
    const double DELTA_T = 0.01;

    auto time_interval = std::chrono::microseconds((int)(DELTA_T * 1e6));

    std::thread t(graphics::start_renderer);

    srand(time(NULL));
    simulated_data simulated_data_;

    for (int tick = 0; true; tick++) {
        auto time_start = std::chrono::high_resolution_clock::now();
        
        if (globals::fired) globals::sequence_time += DELTA_T;
        simulated_data_.update();

        if (tick > 3000) while (std::chrono::high_resolution_clock::now() - time_start < time_interval);
    }
}