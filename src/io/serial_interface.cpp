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
        globals::serial_communications_state = "No open serial ports found. Not opening port.\n";
        globals::globals_mutex.unlock();
        return;
    }

    //std::cout << "open serial ports found:\n";
    //for (std::string port_name : successful_ports) std::cout << port_name << "\n";
    
    if (successful_ports.size() != 1) {
        globals::globals_mutex.lock();
        globals::serial_communications_state = "Multiple open serial ports found. Not opening port.\n";
        globals::globals_mutex.unlock();
        return;
    }

    port = std::make_unique<asio::serial_port>(asio::serial_port(io));
    std::string port_name = successful_ports[0];
    globals::globals_mutex.lock();
    globals::serial_communications_state = "Selecting serial port " + port_name + ".\n";
    globals::globals_mutex.unlock();
    asio::error_code ec;
    port->open(port_name, ec);
}

serial_interface::serial_interface() {}

void serial_interface::write(std::string data) {
    if (!port) find_port();
    if (!port) return;
    globals::globals_mutex.lock();
    globals::console_tx_text += "↑ " + data + "\n";
    globals::globals_mutex.unlock();
    asio::error_code ec;
    asio::write(*port, asio::buffer(data), ec);
    if (ec) {
        //std::cout << "serial_interface: write error: " << ec.message() << "(" << ec.value() << ")\n";
        find_port();
    }
}

std::string serial_interface::read() {
    if (!port) find_port();
    if (!port) return "";
    const size_t max_bytes = 1000000;
    std::vector<char> buffer(max_bytes);
    asio::error_code ec;
    size_t length = port->read_some(asio::buffer(buffer), ec);
    if (ec) {
        //std::cout << "serial_interface: read error: " << ec.message() << "(" << ec.value() << ")\n";
        find_port();
    }
    std::string data = std::string(buffer.data(), length);
    if (data != "") {
        globals::globals_mutex.lock();
        globals::console_rx_text += "↓ " + data + "\n";
        globals::globals_mutex.unlock();
    }
    return data;
}