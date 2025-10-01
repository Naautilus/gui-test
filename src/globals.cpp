#include "globals.h"

bool glfw_is_initialized = false;
image rocket("../images/rocket.png");
data_history history_temperature(1000, 8);
data_history history_pressure(1000, 5);
data_history history_thrust(1000, 1);
data_history history_vibration(1000, 1);
int tick = 0;