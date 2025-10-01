#include "data_channel.h"
#include <iostream>
#include <cmath>
#include <numeric>

    void data_channel::verify_data() {
        if (data.size() != width) {
            std::cerr << "data_channel data size (" << data.size() << ") != width (" << width << "), resetting to double::lowests\n";
            data = std::vector<double>(width, std::numeric_limits<double>::lowest());
        }
        for (double& num : data) {
            if (!std::isnan(num)) continue;
            std::cerr << "data_channel data value is NaN: coercing to double::lowests\n";
            num = std::numeric_limits<double>::lowest();
        }
    }
    data_channel::data_channel(int width_) {
        if (width_ < 1) {
            std::cerr << "bad data_channel creation: width_ = " << width_ << "\n";
            std::abort();
        }
        width = width_;
        data = std::vector<double>(width, std::numeric_limits<double>::lowest());
    }
    data_channel::data_channel(std::vector<double> data_) {
        if (data_.size() < 1) {
            std::cerr << "bad data_channel creation: data_.size() = " << data_.size() << "\n";
            std::abort();
        }
        width = data_.size();
        set_data(data_);
    }
    void data_channel::set_data(std::vector<double> new_data) {
        data = new_data;
        verify_data();
    }
    std::vector<double> data_channel::get_data() {
        verify_data();
        return data;
    }