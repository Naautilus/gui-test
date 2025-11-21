#include "asio.hpp"
#include <string.h>

/*
serial_interface is for communication with a Teensy microcontroller over USB serial.
It assumes that only 1 serial port will be open, which is usually the case.
If there are multiple ports open, it will abort the program.
*/

// TODO: Make this run asynchronously instead of synchronously

class serial_interface {
    asio::io_context io;
    std::unique_ptr<asio::serial_port> port;
    void find_port();
    public:
    serial_interface();
    void write(std::string data);
    std::string read();
};