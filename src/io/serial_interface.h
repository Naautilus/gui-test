#include "asio.hpp"
#include <string.h>

/*
| serial_interface is for communication with a Teensy microcontroller over USB serial.
| It assumes that only 1 serial port will be open, which is usually the case.
| If there are multiple ports open, it will abort the program.
*/

class serial_interface {
    asio::io_context io;
    std::unique_ptr<asio::serial_port> port;
    std::string port_name;
    asio::executor_work_guard<asio::io_context::executor_type> work_guard;
    public:
    void find_port();
    serial_interface();
    void write(std::string data);
    void start_read_loop(std::shared_ptr<std::string> location);
};