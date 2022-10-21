#include "json/json.h"

#include "Args.hpp"
#include "Client.hpp"

#define PORT 5663
#define ADDRESS "10.0.0.106"

std::string SET_PIN(int pin, bool state) {
    std::stringstream ss("SET PIN ");
    ss << pin << ' ' << (state ? "HIGH" : "LOW");
    return ss.str();
}

std::string TCPSend(std::string msg) {
    Client client(ADDRESS, PORT);

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

        return EXIT_SUCCESS;
    }

    if (args.has("--status")) {

        std::cout << TCPSend("SHOWSTR") << std::flush;

        return EXIT_SUCCESS;
    }

    if (args.has("reboot")) {

        TCPSend("REBOOT");

        return EXIT_SUCCESS;
    }

    if (args.has("-on")) {
        int i = args.indexOf("-on");

        if (!args[i + 1].isInt()) {
            std::cerr << "-on: The pin number is invalid" << std::endl;
            return EXIT_FAILURE;
        }

        std::string data = TCPSend(SET_PIN(args[i + 1].ToInt(), true));

        if (data.starts_with("ERROR")) {
            std::cerr << data << std::endl;
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    if (args.has("-off")) {
        int i = args.indexOf("-off");

        if (!args[i + 1].isInt()) {
            std::cerr << "-off: The pin number is invalid" << std::endl;
            return EXIT_FAILURE;
        }

        std::string data = TCPSend(SET_PIN(args[i + 1].ToInt(), false));

        if (data.starts_with("ERROR")) {
            std::cerr << data << std::endl;
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    if (args.has("-s")) {
        int i = args.indexOf("-s");

        if (!args[i + 1].isInt()) {
            std::cerr << "-s: The pin number is invalid" << std::endl;
            return EXIT_FAILURE;
        }

        if (!args[i + 2].isBool() && args[i + 2] != "high" && args[i + 1] != "low") {
            std::cerr << "-s: The state is invalid" << std::endl;
            return EXIT_FAILURE;
        }

        std::string data = TCPSend(SET_PIN(args[i + 1].ToInt(), args[i + 2].ToBool()));

        if (data.starts_with("ERROR")) {
            std::cerr << data << std::endl;
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;

    }

    if (args.has("-t")) {
        int i = args.indexOf("-t");
        
        if (!args[i + 1].isInt()) {
            std::cerr << "-t: The pin number is invalid" << std::endl;
            return EXIT_FAILURE;
        }

        std::string data = TCPSend( "TOGGLE PIN " + args[i + 1].ToString() );

        if (data.starts_with("ERROR")) {
            std::cerr << data << std::endl;
            return EXIT_FAILURE;
        }
    }

    if (args.has("--label")) {
        int i = args.indexOf("--label");

        if (!args[i + 1].isInt()) {
            std::cerr << "--label: The pin number is invalid" << std::endl;
            return EXIT_FAILURE; 
        }

        std::string pin = args[i + 1].ToString();
        std::string label = args[i + 2].ToString();

        std::string data = TCPSend("SET PIN " + pin + " LABEL " + label);

        if (data.starts_with("ERROR")) {
            std::cerr << data << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << label << ": " << pin << std::endl;
    }

    return EXIT_SUCCESS;
}