#pragma once
#include "graphics/image/image.h"
#include "graphics/data/data_history.h"
#include <mutex>

extern image rocket;
extern std::vector<std::pair<float, float>> valve_timings;
extern data_history history_temperature;
extern data_history history_pressure;
extern data_history history_thrust;
extern data_history history_vibration;
extern std::mutex history_mutex;

void initialize_images();