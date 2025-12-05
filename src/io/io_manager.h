#include <string>

/*
| io_manager is a struct which writes and reads commands/data over the serial_interface.
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

struct io_manager {
    static void tx(tx_type tx_type_);
    static void rx();
    static std::string get_checksum(std::string base64_data);
};