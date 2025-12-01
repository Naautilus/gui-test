#include <string>

/*
| io_manager is a class which writes and reads commands/data over the serial_interface.
| To be implemented are commands for:
      [WRITE]
|   - powering on/off the test stand / rocket
|   - starting/stopping the fire sequence
|   - triggering manual purge
|     [READ]
|   - interpreting incoming data
|      - data_rx and writing to data_history globals
|      - error codes, maybe
*/

// comms standard is custom cuz thats what SRAD is about

enum tx_type {
    power_on,
    power_off,
    start_fire,
    stop_fire,
    purge
};

class io_manager {
    std::string rx_for_data;
    std::string rx_for_error_code;
    encode_base64();
    decode_base64();
    public:
    tx(tx_type tx_type_);
    rx();
};