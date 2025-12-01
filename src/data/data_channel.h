#pragma once
#include <vector>

/*
| data_channel is a class which manages a 1D list of values over time.
| It can hold a std::string, double, or valve state.
*/

template <typename T>
class data_channel {
    size_t width;
    std::vector<T> data;
    T reset_value();
    void verify_data_numerics();
    public:
    void verify_data();
    data_channel();
    data_channel(size_t width_);
    data_channel(std::vector<T> data);
    void set_data(std::vector<T> new_data);
    std::vector<T> get_data();
};