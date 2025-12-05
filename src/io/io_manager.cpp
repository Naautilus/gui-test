#include <math.h>
#include <iostream>
#include "io_manager.h"
#include "base64_default_url_unpadded.hpp"
#include <random>
#include <bits/stdc++.h>

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
        Data rows are as follows:
          - (float) Teensy time since startup
          - (float) Thrust 1
          - (float) Vibration 1
          - (float) Temperature 1
          - (float) Temperature 2
          - (float) Temperature 3
          - (float) Temperature 4
          - (float) Temperature 5
          - (float) Temperature 6
          - (float) Temperature 7
          - (float) Temperature 8
          - (float) Pressure 1
          - (float) Pressure 2
          - (float) Pressure 3
          - (float) Pressure 4
          - (float) Pressure 5
          - (float) Pressure 6
          - (float) Pressure 7
          - (float) Pressure 8
          - (float) Valve State 1
          - (float) Valve State 2
          - (float) Valve State 3
          - (float) Valve State 4
          - (float) Valve State 5
          - (float) Valve State 6
          - (float) Valve State 7
          - (float) Valve State 8
          - (float) Valve Manually Actuated? 1
          - (float) Valve Manually Actuated? 2
          - (float) Valve Manually Actuated? 3
          - (float) Valve Manually Actuated? 4
          - (float) Valve Manually Actuated? 5
          - (float) Valve Manually Actuated? 6
          - (float) Valve Manually Actuated? 7
          - (float) Valve Manually Actuated? 8
            
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

static void rx_interpret_tsv_data(std::string tsv_data_base64) {
    
}

static void rx_interpret_error(std::string error_base64) {
    //std::string error_text = base64::decode(error_base64);
    std::cout << "TEENSY ERROR: \"" + error_base64 + "\".\n";
}

}

std::string io_manager::get_tx_message(tx_type tx_type_) {
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

    return message;
}

void io_manager::rx(std::string rx_message) {
    std::vector<std::string> substrings;
    {
        std::stringstream splitter;
        std::string split;
        while (std::getline(splitter, split, '\t')) substrings.push_back(split);
    }
    if (substrings.size() != 3) {
        std::cout << "io_manager::rx: substrings.size() != 3. rx_message: " + rx_message + "\n";
        return;
    }
    std::string type = substrings[0];
    std::string message = substrings[1];
    std::string checksum = substrings[2];
    if (!check_checksum(message, checksum)) return;
    if (type == "DATA") rx_interpret_tsv_data(message);
    else if (type == "ERROR") rx_interpret_error(message);
    else {
        std::cout << "io_manager::rx: invalid message type. rx_message: " + rx_message + "\n";
        return;
    }
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

bool io_manager::check_checksum(std::string base64_in, std::string checksum) {
    std::string calculated_checksum = get_checksum(base64_in);
    if (calculated_checksum == checksum) return true;
    std::cout << "Checksum for base64 string " + base64_in + " is " + calculated_checksum + ", which does not match provided checksum " + checksum + ".\n";
    return false;
}