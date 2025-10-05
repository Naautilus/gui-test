#include "globals.h"

namespace globals {

ImFont* font_default;
ImFont* font_arial;
ImFont* font_deja_vu;
ImFont* font_noto_sans;

bool enable_tx = true;
bool enable_rx = true;
std::optional<std::chrono::high_resolution_clock::time_point> last_rx = std::nullopt;

bool sequence_control_safety = true;
bool fired = false;
double sequence_time = 0.0;
double sequence_max_time = 30.0;
image rocket;

namespace {
    std::vector<std::string> valve_names = {
        "Purge NO2",
        "Purge IPA",
        "Purge N2",
        "Line NO2",
        "Line IPA",
        "Line N2",
        "Igniter"
    };
}

data_channel<valve> valves({
    valve(valve_names[0], -1.0, -1.0), 
    valve(valve_names[1], -1.0, -1.0), 
    valve(valve_names[2], -1.0, -1.0), 
    valve(valve_names[3], 1.0, 10.0), 
    valve(valve_names[4], 1.0, 10.0), 
    valve(valve_names[5], -1.0, 30.0),
    valve(valve_names[6], 3.0, 4.0)
});
data_history history_temperature(3000, 8, data_channel<std::string>({"ENGINE TOP", "", "", "", "", "", "", "ENGINE BOTTOM"}));
data_history history_pressure(3000, 5, data_channel<std::string>({"NO2 TANK", "IPA TANK", "NO2 LINE", "IPA LINE", "CHAMBER"}));
data_history history_thrust(3000, 1, data_channel<std::string>({""}));
data_history history_vibration(3000, 1, data_channel<std::string>({""}));
data_history history_valves(3000, valves.get_data().size(), data_channel<std::string>(valve_names));
std::mutex globals_mutex;
std::vector<logger> loggers;

void initialize_images() {
    rocket = image("../images/rocket.png");
}

}