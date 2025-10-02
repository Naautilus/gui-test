#include "globals.h"

image rocket;
data_history history_temperature(3000, 8, data_channel<std::string>({"ENGINE TOP", "", "", "", "", "", "", "ENGINE BOTTOM"}));
data_history history_pressure(3000, 5, data_channel<std::string>({"NO2 TANK", "IPA TANK", "NO2 LINE", "IPA LINE", "CHAMBER"}));
data_history history_thrust(3000, 1, data_channel<std::string>({""}));
data_history history_vibration(3000, 1, data_channel<std::string>({""}));
std::mutex history_mutex;

void initialize_images() {
    rocket = image("../images/rocket.png");
}