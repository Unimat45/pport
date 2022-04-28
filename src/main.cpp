#include <cmath>
#include <bitset>
#include <fstream>
#include <iostream>
#include <sys/io.h>

#include "Args.hpp"

#define PORT 0x378

// TODO: Find a better way
bArray<int> GetSeparatedPins() {
    unsigned int n = inb(PORT);
    bArray<int> result;
    std::string bits = std::bitset<8>(n).to_string();

    for (short i = bits.size() - 1; i >= 0; i--) {
        if (bits[bits.size() - 1 - i] == '1') {
            result.push(i);
        }
    }
    return result;
}

void WriteData() {
    unsigned int data = inb(PORT);
    std::ofstream file("/etc/pport.data");
    file << data;
    file.close();
}

void SetPin(int pin, bool state) {
    if (pin < 2 || pin > 9) throw std::invalid_argument("Invalid pin number");

    pin -= 2;
    unsigned char d;

    if (state) {
        d = pow(2, pin);
        outb(inb(PORT) | d, PORT);
    }
    else {
        d = 0xFF - pow(2, pin);
        outb(inb(PORT) & d, PORT);
    }
    WriteData();
}

int main(int argc, char** argv) {
    if (ioperm(PORT, 1, 1)) {
        std::cout << "Access denied" << std::endl;
        return EXIT_FAILURE;
    }

    Args args(argc, argv);

    if (args.has("--help")) {
        std::cout
        << "This program is used to control parallel port data pins on a UNIX machine\n"
        << "Available commands:\n"
        << "\t-on <pin>: Sets the provided pin high\n"
        << "\t-off <pin>: Sets the provided pin low\n"
        << "\t-s <pin> <state>: Sets the provided pin in the specified state\n"
        << "\t-t <pin>: Toggles the provided pin\n"
        << "\t--status: Shows all the pins and their state\n"
        << "\treboot: Resets all pins to their last state\n"
        << "\t--help: Shows this information and exit"
        << std::endl;

        return EXIT_SUCCESS;
    }

    if (args.has("--status")) {
        bArray<int> high = GetSeparatedPins();
        for (short i = 2; i < 9; i++) {
            std::cout << i << ": " << (high.Has(i) ? "high" : "low") << std::endl;
        }
        return EXIT_SUCCESS;
    }

    if (args.has("reboot")) {
        std::ifstream file("/etc/pport.data");
        std::string data;
        file >> data;
        outb(std::stoi(data), PORT);

        return EXIT_SUCCESS;
    }

    if (args.has("-on")) {
        int i = args.indexOf("-on");
        int pin = args[i + 1].ToInt();
        SetPin(pin, true);
    }

    if (args.has("-off")) {
        int i = args.indexOf("-off");
        int pin = args[i + 1].ToInt();
        SetPin(pin, false);
    }

    if (args.has("-s")) {
        int i = args.indexOf("-s");
        int pin = args[i + 1].ToInt();
        bool state = args[i + 2] == "high" || args[i + 2].ToBool();
        SetPin(pin, state);
    }

    if (args.has("-t")) {
        int i = args.indexOf("-s");
        int pin = args[i + 1].ToInt();
        bArray<int> high = GetSeparatedPins();
        SetPin(pin, !high.Has(pin));
    }
}