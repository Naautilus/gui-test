#include "data_channel.h"
#include <iostream>
#include <cmath>
#include <numeric>

template <>
double data_channel<double>::reset_value() {
    return std::numeric_limits<double>::lowest();
}

template <>
std::string data_channel<std::string>::reset_value() {
    return "";
}

template <>
void data_channel<double>::verify_data() {
    if (data.size() != width) {
        std::cerr << "data_channel data size (" << data.size() << ") != width (" << width << "), resetting values\n";
        data = std::vector<double>(width, reset_value());
    }
    for (double& num : data) {
        if (!std::isnan(num)) continue;
        std::cerr << "data_channel data value is NaN: coercing to double::lowests\n";
        num = std::numeric_limits<double>::lowest();
    }
}

template <>
void data_channel<std::string>::verify_data() {
    if (data.size() != width) {
        std::cerr << "data_channel data size (" << data.size() << ") != width (" << width << "), resetting values\n";
        data = std::vector<std::string>(width, reset_value());
    }
}

template <typename T>
data_channel<T>::data_channel() {}

template <typename T>
data_channel<T>::data_channel(size_t width_) {
    if (width_ < 1) {
        std::cerr << "bad data_channel creation: width_ = " << width_ << "\n";
        std::abort();
    }
    width = width_;
    data = std::vector<T>(width, reset_value());
}

template <typename T>
data_channel<T>::data_channel(std::vector<T> data_) {
    if (data_.size() < 1) {
        std::cerr << "bad data_channel creation: data_.size() = " << data_.size() << "\n";
        std::abort();
    }
    width = data_.size();
    set_data(data_);
}

template <typename T>
void data_channel<T>::set_data(std::vector<T> new_data) {
    data = new_data;
    verify_data();
}

template <typename T>
std::vector<T> data_channel<T>::get_data() {
    verify_data();
    return data;
}

template struct data_channel<double>;
template struct data_channel<std::string>;