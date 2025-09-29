#include <iostream>
#include <string>
#include <chrono>

// https://gist.github.com/ConnerWill/d4b6c776b509add763e17f9f113fd25b

void wait(double millis) {
    auto start = std::chrono::high_resolution_clock::now();
    while (std::chrono::high_resolution_clock::now() - start < std::chrono::milliseconds(1000)) {};
}

int main() {
    char escape = 27;

    std::cout << escape << "[H"; // go to origin
    std::cout << escape << "[J"; // clear below cursor

    wait(1000);

    std::cout << "abcdefg\n" 
              << "hijklmn\n"
              << "opqrstu\n"
              << "vwxyz  ";

    wait(1000);

    std::cout << escape << "[3A";

    wait(1000);

    std::cout << escape << "[H"; // go to origin
    std::cout << escape << "[J"; // clear below cursor

}