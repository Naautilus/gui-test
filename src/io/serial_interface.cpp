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
        std::cout << "serial_interface: No open serial ports found. Not opening port.\n";
        return;
    }

    std::cout << "serial_interface: open serial ports found:\n";
    for (std::string port_name : successful_ports) std::cout << port_name << "\n";
    
    if (successful_ports.size() != 1) {
        std::cout << "serial_interface: Multiple open serial ports found. Not opening port.\n";
        return;
    }

    port = std::make_unique<asio::serial_port>(asio::serial_port(io));
    std::string port_name = successful_ports[0];
    std::cout << "serial_interface: Selecting serial port " << port_name << ".\n";
    port->open(port_name);
}

serial_interface::serial_interface() {}

void serial_interface::write(std::string data) {
    if (!port) find_port();
    if (!port) return;
    globals::serial_communications += "↑ ";
    globals::serial_communications += data;
    globals::serial_communications += "\n";
    asio::error_code ec;
    asio::write(*port, asio::buffer(data), ec);
    if (ec) {
        std::cout << "serial_interface: write error: " << ec.message() << "(" << ec.value() << ")\n";
        find_port();
    }
}

std::string serial_interface::read() {
    if (!port) find_port();
    if (!port) return "";
    const size_t max_bytes = 1024;
    std::vector<char> buffer(max_bytes);
    asio::error_code ec;
    size_t length = port->read_some(asio::buffer(buffer), ec);
    if (ec) {
        std::cout << "serial_interface: read error: " << ec.message() << "(" << ec.value() << ")\n";
        find_port();
    }
    std::string data = std::string(buffer.data(), length);
    globals::serial_communications += "↓ ";
    globals::serial_communications += data;
    globals::serial_communications += "\n";
    return data;
}