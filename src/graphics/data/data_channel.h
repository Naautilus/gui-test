#pragma once
#include <vector>

template <typename T>
class data_channel {
    size_t width;
    std::vector<T> data;
    T reset_value();
    public:
    void verify_data();
    data_channel();
    data_channel(size_t width_);
    data_channel(std::vector<T> data);
    void set_data(std::vector<T> new_data);
    std::vector<T> get_data();
};