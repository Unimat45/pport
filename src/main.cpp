#include <cmath>
#include <fstream>
#include <iostream>
#include <sys/io.h>

#include "Args.hpp"

#define PORT 0x378

bArray<int> GetHighPins() {
    unsigned int n = inb(PORT);
    bArray<int> result;
    for (short i = 0; i <= 8; i++) {
        if (fmod((double)n / 2 , 1) != 0) {
            result.push(i + 2);
        }
        n /= 2;
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

    if (args.has("--help") || args.count() == 0) {
        std::cout
        << "This program is used to control parallel port data pins on a UNIX machine\n"
        << "Available commands:\n"
        << "\treboot: Resets all pins to their last state\n"
        << "\t-on <pin>: Sets the provided pin high\n"
        << "\t-off <pin>: Sets the provided pin low\n"
        << "\t-s <pin> <state>: Sets the provided pin in the specified state\n"
        << "\t-t <pin>: Toggles the provided pin\n"
        << "\t--status: Shows all the pins and their state\n"
        << "\t--help: Shows this information and exit\n"
        << "Available pin numbers are: 2, 3, 4, 5, 6, 7, 8, 9\n"
        << "Available states are: true, false, 1, 0, high, low"
        << std::endl;

        return EXIT_SUCCESS;
    }

    if (args.has("--status")) {
        bArray<int> high = GetHighPins();
        for (short i = 2; i < 10; i++) {
            std::cout << "pin " << i << ": " << (high.Has(i) ? "high" : "low") << std::endl;
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
        if (!args[i + 1].isInt()) {
            std::cerr << "-on: The pin number is invalid" << std::endl;
            return EXIT_FAILURE;
        }
        int pin = args[i + 1].ToInt();
        SetPin(pin, true);
    }

    if (args.has("-off")) {
        int i = args.indexOf("-off");
        if (!args[i + 1].isInt()) {
            std::cerr << "-off: The pin number is invalid" << std::endl;
            return EXIT_FAILURE;
        }
        int pin = args[i + 1].ToInt();
        SetPin(pin, false);
    }

    if (args.has("-s")) {
        int i = args.indexOf("-s");
        if (!args[i + 1].isInt()) {
            std::cerr << "-s: The pin number is invalid" << std::endl;
            return EXIT_FAILURE;
        }
        int pin = args[i + 1].ToInt();
        if (!args[i + 2].isBool() && args[i + 2] != "high" && args[i + 1] != "low") {
            std::cerr << "-s: The state is invalid" << std::endl;
            return EXIT_FAILURE;
        }
        bool state = args[i + 2] == "high" || args[i + 2].ToBool();
        SetPin(pin, state);
    }

    if (args.has("-t")) {
        int i = args.indexOf("-t");
        if (!args[i + 1].isInt()) {
            std::cerr << "-t: The pin number is invalid" << std::endl;
            return EXIT_FAILURE;
        }
        int pin = args[i + 1].ToInt();
        bArray<int> high = GetHighPins();
        SetPin(pin, !high.Has(pin));
    }

    return EXIT_SUCCESS;
}