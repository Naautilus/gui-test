#pragma once
#include "../graphics/image/image.h"
#include "../data/data_history.h"
#include "../data/valve.h"
#include "../logger/logger.h"
#include "imgui.h"
#include <mutex>
#include <chrono>
#include <optional>

namespace globals {

extern ImFont* font_default;
extern ImFont* font_arial;
extern ImFont* font_deja_vu;
extern ImFont* font_noto_sans;

extern bool power;
extern bool power_safety;
extern std::mutex globals_mutex;
extern bool enable_tx;
extern bool enable_rx;
extern std::optional<std::chrono::high_resolution_clock::time_point> last_rx;
extern bool sequence_control_safety;
extern bool fired;
extern double sequence_time;
extern double sequence_max_time;
extern std::string console_text;
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