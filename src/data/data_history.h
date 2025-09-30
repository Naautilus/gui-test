#pragma once
#include "data_channel.h"

class data_history {
    int length; // how many historical datapoints to keep
    int width; // how many datapoints wide the data is
    std::vector<data_channel> history;
    void verify_data();
    public:
    data_history(int length_);
    void update_data(data_channel new_data);
    data_channel get_current_data();
    std::vector<data_channel> get_data_history();
};