#include "valve.h"

valve::valve() {}

valve::valve(std::string name_) {
    name = name_;
}

valve::valve(std::string name_, double open_time_, double close_time_) {
    name = name_;
    open_time = open_time_;
    close_time = close_time_;
}

bool valve::is_open(double time) {
    if (manual_activation_state) return manual_activation_state.value();
    if (time < open_time) return false;
    if (time > close_time) return false;
    return true;
}

std::string valve::get_name() {
    return name;
}

void valve::set_to_manual_activation(bool activated) {
    manual_activation_state = std::make_optional(activated);
}

void valve::set_to_automatic_activation() {
    manual_activation_state = std::nullopt;
}