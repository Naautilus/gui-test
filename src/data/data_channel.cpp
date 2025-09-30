#pragma once
#include "data_channel.h"
#include <iostream>
#include <cmath>

class data_channel {
    int width;
    std::vector<double> data;
    public:
    void verify_data() {
        if (data.size() != width) {
            std::cerr << "data_channel data size (" << data.size() << ") != width (" << width << "), resetting to 0's\n";
            data = std::vector<double>(width, 0);
        }
        for (double& num : data) {
            if (!std::isnan(num)) continue;
            std::cerr << "data_channel data value is NaN: coercing to 0\n";
            num = 0;
        }
    }
    data_channel(int width_) {
        if (width_ < 1) {
            std::cerr << "bad data_channel creation: width = " << width_ << "\n";
            std::abort();
        }
        width = width_;
        data = std::vector<double>(width, 0);
    }
    void set_data(std::vector<double> new_data) {
        data = new_data;
        verify_data();
    }
    std::vector<double> get_data() {
        verify_data();
        return data;
    }
};