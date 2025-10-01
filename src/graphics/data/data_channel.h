#pragma once
#include <vector>

class data_channel {
    int width;
    std::vector<double> data;
    public:
    void verify_data();
    data_channel(int width_);
    data_channel(std::vector<double> data);
    void set_data(std::vector<double> new_data);
    std::vector<double> get_data();
};