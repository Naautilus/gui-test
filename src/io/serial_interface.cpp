#include "serial_interface.h"
#include "../globals/globals.h"
#include <iostream>
#include <vector>

void serial_interface::find_port() {
    std::vector<std::string> possible_port_names;
    std::vector<std::string> successful_ports;

    // windows (tested)
    for (int i = 1; i <= 256; i++) possible_port_names.push_back("COM" + std::to_string(i));
    // linux (untested but.. leave it in >:] )
    for (int i = 0; i < 256; i++) possible_port_names.push_back("ttyUSB" + std::to_string(i));
    
    for (std::string port_name_ : possible_port_names) {
        asio::serial_port port_(io);
        asio::error_code ec;
        port_.open(port_name_, ec);
        if (!ec) {
            successful_ports.push_back(port_name_);
        }
    }

    if (successful_ports.size() == 0) {
        globals::globals_mutex.lock();
        globals::serial_communications_state = "No open serial ports found\n\n";
        globals::globals_mutex.unlock();
        return;
    }

    if (successful_ports.size() != 1) {
        globals::globals_mutex.lock();
        globals::serial_communications_state = "Multiple open serial ports found\n(";
        for (int i = 0; i < successful_ports.size(); i++) {
            globals::serial_communications_state += successful_ports[i];
            if (i != successful_ports.size() - 1) globals::serial_communications_state += ", ";
            else globals::serial_communications_state += ")\n";
        }
        globals::globals_mutex.unlock();
        return;
    }

    port = std::make_unique<asio::serial_port>(asio::serial_port(io));
    port_name = successful_ports[0];
    globals::globals_mutex.lock();
    globals::serial_communications_state = "Selecting serial port " + port_name + "\n\n";
    globals::globals_mutex.unlock();
    asio::error_code ec;
    port->open(port_name, ec);
}

serial_interface::serial_interface() : work_guard(asio::make_work_guard(io)) {
    std::thread t(&asio::io_context::run, &io);
    t.detach();
}

void serial_interface::write(std::string data) {
    if (!port) find_port();
    if (!port) return;
    globals::globals_mutex.lock();
    globals::console_tx_text += data;
    globals::globals_mutex.unlock();
    asio::error_code ec;
    asio::async_write(*port, asio::buffer(data),
        [this](std::error_code ec, std::size_t size) {
            if (ec) {
                find_port();
            } else {
                globals::globals_mutex.lock();
                globals::serial_communications_state = "Using serial port " + port_name + "\n\n";
                globals::globals_mutex.unlock();
            }
        });
}

void serial_interface::start_read_loop(std::shared_ptr<std::string> location) {
    if (!port) find_port();
    if (!port) return;
    auto read_buffer = std::make_shared<std::array<char, 128>>();
    port->async_read_some(asio::buffer(*read_buffer),
    [this, location, read_buffer](std::error_code ec, std::size_t size) {
        if (ec) {
            find_port();
        }
        else {
            std::string data(read_buffer->data(), size);
            if (data != "") {
                if (location) *location += data;
                globals::globals_mutex.lock();
                globals::last_rx = std::make_optional(std::chrono::high_resolution_clock::now());
                globals::serial_communications_state = "Using serial port " + port_name + "\n\n";
                globals::console_rx_text += data;
                globals::globals_mutex.unlock();
            }
        }
        start_read_loop(location);
    });
}