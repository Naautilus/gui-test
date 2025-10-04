#include <thread>
#include "math.h"
#include "graphics/renderer.h"
#include "globals.h"

int main() {
    const double DELTA_T = 0.01;

    auto time_interval = std::chrono::microseconds((int)(DELTA_T * 1e6));
    std::thread t(start_renderer);

    std::vector<double> simulated_pressure;
    std::vector<double> simulated_temperature;
    std::vector<double> simulated_thrust;
    std::vector<double> simulated_vibration;

    double base_simulated_pressure_high;
    double base_simulated_pressure_low;
    double base_simulated_temperature;
    double base_simulated_thrust;
    double base_simulated_vibration;

    std::vector<double> weighted_simulated_pressure(globals::history_pressure.get_width(), 0);
    std::vector<double> weighted_simulated_temperature(globals::history_temperature.get_width(), 0);
    std::vector<double> weighted_simulated_thrust(globals::history_thrust.get_width(), 0);
    std::vector<double> weighted_simulated_vibration(globals::history_vibration.get_width(), 0);

    srand(time(NULL));

    for (int tick = 0; true; tick++) {

        if (globals::fired) globals::sequence_time += DELTA_T;

        if (globals::sequence_time <= 0) {
            base_simulated_pressure_high = 800;
            base_simulated_pressure_low = 0;
            base_simulated_temperature = 20;
            base_simulated_thrust = 0;
            base_simulated_vibration = 0;
        } else if (globals::sequence_time < 3) {
            base_simulated_pressure_high = 800;
            base_simulated_pressure_low = 300 * log(globals::sequence_time + 1);
            base_simulated_temperature = 100 * log(globals::sequence_time + 1) + 20;
            base_simulated_thrust = 1 * globals::sequence_time;
            base_simulated_vibration = ((globals::sequence_time + 2) * (5-globals::sequence_time)) * 8 + 10;
        } else if (globals::sequence_time < 10) {
            base_simulated_pressure_high = -10 * globals::sequence_time + 800;
            base_simulated_pressure_low = -10 * globals::sequence_time + 600;
            base_simulated_temperature = 400 * log(globals::sequence_time + 1) - 400;
            base_simulated_thrust = -0.1 * globals::sequence_time + 6;
            base_simulated_vibration = -5 * globals::sequence_time + 50;
        } else {
            base_simulated_pressure_high = 700;
            base_simulated_pressure_low = 0;
            base_simulated_temperature = -5 * globals::sequence_time + 500;
            base_simulated_thrust = 0;
            base_simulated_vibration = 0;
        }

        double avg_factor = 0.005;

        for (double& num : weighted_simulated_pressure) num *= (1 - avg_factor);
        for (double& num : weighted_simulated_temperature) num *= (1 - avg_factor);
        for (double& num : weighted_simulated_thrust) num *= (1 - avg_factor);
        for (double& num : weighted_simulated_vibration) num *= (1 - avg_factor);

        for (int i = 0; i < weighted_simulated_pressure.size(); i++) {
            double base_simulated_pressure;
            if (i == 0) base_simulated_pressure = base_simulated_pressure_high;
            if (i == 1) base_simulated_pressure = base_simulated_pressure_high * 0.95;
            if (i == 2) base_simulated_pressure = base_simulated_pressure_high * 0.50 + base_simulated_pressure_low * 0.50;
            if (i == 3) base_simulated_pressure = base_simulated_pressure_high * 0.50 + base_simulated_pressure_low * 0.50;
            if (i == 4) base_simulated_pressure = base_simulated_pressure_low;
            weighted_simulated_pressure[i] += avg_factor * base_simulated_pressure * 1 + (rand() % 100 - 50) * 0.05;

        }
        for (double& num : weighted_simulated_temperature) num += avg_factor * base_simulated_temperature * 1 + (rand() % 100 - 50) * 0.01;
        for (double& num : weighted_simulated_thrust) num += avg_factor * base_simulated_thrust * 1 + (rand() % 100 - 50) * 0.0005;
        for (double& num : weighted_simulated_vibration) num += avg_factor * base_simulated_vibration * 1 + (rand() % 100 - 50) * 0.01;
        
        simulated_pressure = weighted_simulated_pressure;
        simulated_temperature = weighted_simulated_temperature;
        simulated_thrust = weighted_simulated_thrust;
        simulated_vibration = weighted_simulated_vibration;

        for (int i = 0; i < 8; i++) simulated_temperature[i] = weighted_simulated_temperature[i] * (3.0 - i / 8.0) / 3.0;

        auto time_start = std::chrono::high_resolution_clock::now();

        {
            std::lock_guard<std::mutex> lock(globals::history_mutex);
            globals::history_pressure.update_data(data_channel(simulated_pressure));
            globals::history_temperature.update_data(data_channel(simulated_temperature));
            globals::history_thrust.update_data(data_channel(simulated_thrust));
            globals::history_vibration.update_data(data_channel(simulated_vibration));

            std::vector<double> valves_as_doubles;
            std::vector<valve> valves_ = globals::valves.get_data();
            for (valve& v : valves_) valves_as_doubles.push_back(v.is_open(globals::sequence_time) ? 1.0 : 0.0);
            globals::history_valves.update_data(data_channel(valves_as_doubles));
        }

        if (tick > 3000) while (std::chrono::high_resolution_clock::now() - time_start < time_interval);
    }
}