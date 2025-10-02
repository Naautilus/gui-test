#include "globals.h"

image rocket;
data_history history_temperature(3000, 8);
data_history history_pressure(3000, 5);
data_history history_thrust(3000, 1);
data_history history_vibration(3000, 1);
std::mutex history_mutex;

void initialize_images() {
    rocket = image("../images/rocket.png");
}