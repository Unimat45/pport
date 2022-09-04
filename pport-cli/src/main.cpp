#include <iostream>
#include <sstream>

#include "json/json.h"

#include "Args.hpp"
#include "Client.hpp"

#define PORT 5663

std::string TCPSend(std::string msg) {
    Client client("10.0.0.106", PORT);

    client.Write(msg);

    std::string result = client.Read();

    client.Close();

    return result;
}

int main(int argc, char** argv) {
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

        return (EXIT_FAILURE);
    }

    if (args.has("--status")) {

        Json::Value root;
        std::stringstream ss(TCPSend("SHOW"));

        ss >> root;

        for (size_t i = 0; i < root.size(); i++) {
            Json::Value current = root[(int)i];

            std::cout << "pin " << current["pin"] << ": " << (current["state"].asBool() ? "on" : "off") << std::endl;
        }

        return (EXIT_SUCCESS);
    }

    if (args.has("reboot")) {

        TCPSend("REBOOT");

        return (EXIT_SUCCESS);
    }

    if (args.has("-on")) {
        int i = args.indexOf("-on");
        if (!args[i + 1].isInt()) {
            std::cerr << "-on: The pin number is invalid" << std::endl;
            return (EXIT_FAILURE);
        }

        TCPSend("SET PIN " + args[i + 1].ToString() + " HIGH");

        return (EXIT_SUCCESS);
    }

    if (args.has("-off")) {
        int i = args.indexOf("-off");
        if (!args[i + 1].isInt()) {
            std::cerr << "-off: The pin number is invalid" << std::endl;
            return (EXIT_FAILURE);
        }

        TCPSend("SET PIN " + args[i + 1].ToString() + " LOW");

        return (EXIT_SUCCESS);
    }

    if (args.has("-s")) {
        int i = args.indexOf("-s");
        if (!args[i + 1].isInt()) {
            std::cerr << "-s: The pin number is invalid" << std::endl;
            return (EXIT_FAILURE);
        }
        int pin = args[i + 1].ToInt();
        if (!args[i + 2].isBool() && args[i + 2] != "high" && args[i + 1] != "low") {
            std::cerr << "-s: The state is invalid" << std::endl;
            return (EXIT_FAILURE);
        }

        TCPSend("SET PIN " + args[i + 1].ToString() + ' ' + (args[i + 2].ToBool() ? "HIGH" : "LOW"));

        return (EXIT_SUCCESS);

    }

    if (args.has("-t")) {
        int i = args.indexOf("-t");
        
        if (!args[i + 1].isInt()) {
            std::cerr << "-t: The pin number is invalid" << std::endl;
            return (EXIT_FAILURE);
        }

        int pin = args[i + 1].ToInt();
        std::string str_pin = args[i + 1].ToString();

        std::stringstream pin_data( TCPSend("SHOW PIN " + str_pin) );

        Json::Value root;

        pin_data >> root;

        std::string newState = root["state"].asBool() ? "LOW" : "HIGH";

        TCPSend("SET PIN " + str_pin + ' ' + newState);
    }

    return EXIT_SUCCESS;
}