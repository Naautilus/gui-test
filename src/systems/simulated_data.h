#pragma once
#include <vector>

/*
| simulated_data creates fake simulation data for testing data logging.
| Make sure to switch to the real data before launch. :)
*/

class simulated_data {
    std::vector<double> simulated_pressure;
    std::vector<double> simulated_temperature;
    std::vector<double> simulated_thrust;
    std::vector<double> simulated_vibration;
    double base_simulated_pressure_high;
    double base_simulated_pressure_low;
    double base_simulated_temperature;
    double base_simulated_thrust;
    double base_simulated_vibration;
    std::vector<double> weighted_simulated_pressure;
    std::vector<double> weighted_simulated_temperature;
    std::vector<double> weighted_simulated_thrust;
    std::vector<double> weighted_simulated_vibration;
    public:
    simulated_data();
    void update();
};