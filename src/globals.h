#pragma once
#include "graphics/image/image.h"
#include "data/data_history.h"
#include "data/valve.h"
#include <mutex>

namespace globals {

extern bool fired;
extern double time;
extern image rocket;
extern data_channel<valve> valves;
extern data_history history_temperature;
extern data_history history_pressure;
extern data_history history_thrust;
extern data_history history_vibration;
extern data_history history_valves;
extern std::mutex history_mutex;

void initialize_images();

}