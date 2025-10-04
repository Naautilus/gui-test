#pragma once
#include <string>
#include "data_channel.h"

class data_history {
    int length; // how many historical datapoints to keep
    int width; // how many datapoints wide the data is
    std::vector<data_channel<double>> history;
    data_channel<std::string> labels;
    public:
    void verify_data();
    data_history(int length_, int width_, data_channel<std::string> labels_);
    void update_data(data_channel<double> new_data);
    data_channel<double> get_current_data();
    std::vector<data_channel<double>> get_data_history();
    data_channel<std::string> get_labels();
    std::vector<double> get_1d_vector();
    int get_length();
    int get_width();
};