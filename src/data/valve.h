#pragma once
#include <string>
#include <optional>

class valve {
    std::string name;
    std::optional<bool> manual_activation_state;
    public:
    double open_time;
    double close_time;
    valve();
    valve(std::string name_);
    valve(std::string name_, double open_time_, double close_time_);
    bool is_open(double time);
    std::string get_name();
    void set_to_manual_activation(bool activated);
    void set_to_automatic_activation();
};