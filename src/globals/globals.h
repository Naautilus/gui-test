#pragma once
#include "../graphics/image/image.h"
#include "../data/data_history.h"
#include "../data/valve.h"
#include "../logger/logger.h"
#include <mutex>
#include <chrono>
#include <optional>

namespace globals {

extern std::mutex globals_mutex;
extern bool enable_tx;
extern bool enable_rx;
extern std::optional<std::chrono::high_resolution_clock::time_point> last_rx;
extern bool fired;
extern double sequence_time;
extern image rocket;
extern data_channel<valve> valves;
extern data_history history_temperature;
extern data_history history_pressure;
extern data_history history_thrust;
extern data_history history_vibration;
extern data_history history_valves;
extern std::vector<logger> loggers;

void initialize_images();

}