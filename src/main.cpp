#include <thread>
#include "math.h"
#include "graphics/renderer.h"
#include "globals.h"

int main() {
    auto time_interval = std::chrono::milliseconds(10);
    std::thread t(start_renderer);
    int tick = 0;
    while (true) {
        auto time_start = std::chrono::high_resolution_clock::now();
        {
            std::lock_guard<std::mutex> lock(history_mutex);
            int width = history_temperature.get_width();
            data_channel rocket_current_data(history_temperature.get_width());
            std::vector<double> data;
            for (int i = 0; i < width; i++) data.push_back(sin(0.01 * (i+1) * tick));
            rocket_current_data.set_data(data);
            history_temperature.update_data(rocket_current_data);
        }
    
        {
            std::lock_guard<std::mutex> lock(history_mutex);
            int width = history_pressure.get_width();
            data_channel rocket_current_data(history_pressure.get_width());
            std::vector<double> data;
            for (int i = 0; i < width; i++) data.push_back(sin(0.01 * (i+1) * tick));
            rocket_current_data.set_data(data);
            history_pressure.update_data(rocket_current_data);
        }
    
        {
            std::lock_guard<std::mutex> lock(history_mutex);
            int width = history_thrust.get_width();
            data_channel rocket_current_data(history_thrust.get_width());
            std::vector<double> data;
            for (int i = 0; i < width; i++) data.push_back(sin(0.01 * (i+1) * tick));
            rocket_current_data.set_data(data);
            history_thrust.update_data(rocket_current_data);
        }
    
        {
            std::lock_guard<std::mutex> lock(history_mutex);
            int width = history_vibration.get_width();
            data_channel rocket_current_data(history_vibration.get_width());
            std::vector<double> data;
            for (int i = 0; i < width; i++) data.push_back(sin(0.01 * (i+1) * tick));
            rocket_current_data.set_data(data);
            history_vibration.update_data(rocket_current_data);
        }
        while (std::chrono::high_resolution_clock::now() - time_start < time_interval);
        tick++;
    }
}