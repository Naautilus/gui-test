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
    
    for (std::string port_name : possible_port_names) {
        asio::serial_port port_(io);
        asio::error_code ec;
        port_.open(port_name, ec);
        if (!ec) {
            successful_ports.push_back(port_name);
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
    std::string port_name = successful_ports[0];
    globals::globals_mutex.lock();
    globals::serial_communications_state = "Selecting serial port " + port_name + "\n\n";
    globals::globals_mutex.unlock();
    asio::error_code ec;
    port->open(port_name, ec);
}

serial_interface::serial_interface() {}

void serial_interface::write(std::string data) {/*
    if (!port) find_port();
    if (!port) return;
    globals::globals_mutex.lock();
    globals::console_tx_text += "↑ " + data + "\n";
    globals::globals_mutex.unlock();
    asio::error_code ec;
    asio::write(*port, asio::buffer(data), ec);
    if (ec) {
        find_port();
    }
*/}

void serial_interface::read(/*std::shared_ptr<std::string> write_destination*/) {/*
    if (!port) find_port();
    if (!port) return;
    auto read_buffer = std::make_shared<std::array<char, 1'000'000>>();
    port->async_read_some(asio::buffer(*read_buffer),
    [this, write_destination, read_buffer](std::error_code ec, std::size_t size) {
        if (ec) {
            find_port();
        }
        else {
            std::string data(read_buffer->data(), size);
            if (data != "") {
                globals::globals_mutex.lock();
                globals::console_rx_text += "↓ " + data + "\n";
                globals::globals_mutex.unlock();
            }
            if (write_destination) *write_destination += data;
        }
    });
*/}