#pragma once
#include "data_channel.h"
#include <iostream>
#include <cmath>

class data_history {
    int length;
    int width;
    std::vector<data_channel> history;
    void verify_data() {
        if (history.size() != length) {
            std::cerr << "data_history history size (" << history.size() << ") != length (" << length << "), resetting to 0's\n";
            history = std::vector<data_channel>(length, data_channel(width));
        }
        for (data_channel& data_channel_ : history) {
            data_channel_.verify_data();
        }
    }
    public:
    data_history(int length_, int width_) {
        if (length_ < 1) {
            std::cerr << "bad data_history creation: length = " << length_ << "\n";
            std::abort();
        }
        if (width_ < 1) {
            std::cerr << "bad data_history creation: width = " << width_ << "\n";
            std::abort();
        }
        length = length_;
        width = width_;
        history = std::vector<data_channel>(length, data_channel(width));
    }
    void update_data(data_channel new_data) {
        for (int i = length - 1; i > 0; i--) {
            history[i] = history[i-1];
        }
        history[0] = new_data;
        verify_data();
    }
    data_channel get_current_data() {
        verify_data();
        return history[0];
    }
    std::vector<data_channel> get_data_history() {
        verify_data();
        return history;
    }
};