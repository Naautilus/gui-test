#include "data_history.h"
#include <iostream>
#include <cmath>

void data_history::verify_data() {
    if (history.size() != length) {
        std::cerr << "data_history history size (" << history.size() << ") != length (" << length << "), resetting to 0's\n";
        history = std::vector<data_channel<double>>(length, data_channel<double>(width));
    }
    for (data_channel<double>& data_channel_ : history) {
        data_channel_.verify_data();
    }
}

data_history::data_history(int length_, int width_, data_channel<std::string> labels_) {
    if (length_ < 1) {
        std::cerr << "bad data_history creation: length = " << length_ << "\n";
        std::abort();
    }
    if (width_ < 1) {
        std::cerr << "bad data_history creation: width = " << width_ << "\n";
        std::abort();
    }
    if (labels_.get_data().size() == 0) labels_ = data_channel<std::string>(width_);
    if (labels_.get_data().size() != width_) {
        std::cerr << "bad data_history creation: labels_.get_data().size() (" << labels_.get_data().size() << ") != width (" << width_ << ")\n";
        std::abort();
    }
    length = length_;
    width = width_;
    labels = labels_;
    history = std::vector<data_channel<double>>(length, data_channel<double>(width));
}

void data_history::update_data(data_channel<double> new_data) {
    for (int i = length - 1; i > 0; i--) {
        history[i] = history[i-1];
    }
    history[0] = new_data;
    verify_data();
}

data_channel<double> data_history::get_current_data() {
    verify_data();
    return history[0];
}

std::vector<data_channel<double>> data_history::get_data_history() {
    verify_data();
    return history;
}

std::vector<double> data_history::get_1d_vector() {
    std::vector<double> output(length * width, std::numeric_limits<double>::lowest());
    for (int i = 0; i < length; i++) {
        std::vector<double> data = history[i].get_data();
        for (int j = 0; j < width; j++) {
            output[i + length * j] = data[j];
        }
    }
    return output;
}

int data_history::get_length() {
    return length;
}

int data_history::get_width() {
    return width;
}