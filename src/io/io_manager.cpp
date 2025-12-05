#include <math.h>
#include <iostream>
#include "io_manager.h"
#include "base64_default_url_unpadded.hpp"
#include <random>

/*

COMMS FORMAT

All messages follow this format:

    FIRE[\t]7iNYUnEq8Mo4FcdHmpSH3A[\t]ip7d[\n]
    ^^^^    ^^^^^^^^^^^^^^^^^^^^^^    ^^^^
     |       |                         |
     |       |                        4-digit checksum of data in ASCII->base64.
     |       |
     |      Message data in ASCII->base64.
     |
   Type of message being delivered.


Types of messages:
    computer -> teensy:
    PWRON   nEq8   nEq8
    PWROFF  cdHm   cdHm
    FIRE    iNYU   iNYU
    ABORT   pSH3   pSH3
    PURGE   4Fcd   4Fcd
        For all 5 of these, the message is a random string.
        This command will send on repeat to the Teensy
        until the Teensy correctly returns the same command,
        as a receipt.

    teensy -> computer:
    DATA 61vBYGG1lRztZaOSIsYZxXcuYW8fzE/z hJ7d
        The message is a tsv containing rows of telemetry data.
    ERROR y5PBMkFGOp f7Tb
        The message is a block of text describing the error.

*/

namespace {
    static int base64_max_for_length(int length) {
        return (1 << (length * 6));
    }
    static int base64_bytes_for_length(int length) {
        return ceil((6 * length) / 8);
    }
}

void io_manager::tx(tx_type tx_type_) {
    const int RANDOM_SIZE = 4;

    int random_value = std::mt19937(std::random_device{}())();
    std::vector<uint8_t> bytes(base64_bytes_for_length(RANDOM_SIZE));
    memcpy(bytes.data(), &random_value, base64_bytes_for_length(RANDOM_SIZE));
    std::string random_base64 = base64::encode(bytes);

    std::string message;
    
    switch(tx_type_) {
        case power_on:   message = "PWRON\t"; break;
        case power_off:  message = "PWROFF\t"; break;
        case start_fire: message = "FIRE\t"; break;
        case stop_fire:  message = "ABORT\t"; break;
        case purge:      message = "PURGE\t"; break;
    }
    message += random_base64 + "\t" + get_checksum(random_base64);

    std::cout << message << "\n";

}

std::string io_manager::get_checksum(std::string base64_in) {
    const int SIZE = 4;

    // base64 -> bytes -> sum -> bytes -> base64
    int sum = 0;
    std::vector<uint8_t> bytes_in = base64::decode(base64_in);
    for (uint8_t byte : bytes_in) sum = (sum + byte) % base64_max_for_length(SIZE);
    std::vector<uint8_t> bytes_out(base64_bytes_for_length(SIZE));
    memcpy(bytes_out.data(), &sum, base64_bytes_for_length(SIZE));
    return base64::encode(bytes_out);
}