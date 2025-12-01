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

class io_manager {
    std::string rx_for_data;
    std::string rx_for_error_code;
    public:
    tx_power_on();
    tx_power_off();
    tx_start_fire();
    tx_stop_fire();
    tx_purge();
    rx_interpret_data();
};