#include "Command.h"
#include "CommandException.hpp"
#include "Parallel.h"
#include "PinException.hpp"
#include "str_utils.h"

#include <algorithm>
#include <sstream>
#include <vector>

std::vector<std::string> split_string(std::string &s, const char *separator) {
    std::vector<std::string> splitted;
    size_t start_at = 0, found_at = 0;

    while ((found_at = s.find(separator, start_at)) != std::string::npos) {
        splitted.push_back(s.substr(start_at, found_at - start_at));
        start_at = found_at + 1;
    }
    splitted.push_back(s.substr(start_at));

    return splitted;
}

std::string vector_join(std::vector<std::string> &vec,
                        const char *delimiter = " ") {
    std::stringstream sb;

    for (size_t i = 0; i < vec.size(); i++) {
        sb << vec[i];
        if (i < vec.size() - 1) {
            sb << delimiter;
        }
    }

    return sb.str();
}

Command::Command(std::string command) { this->tokenize(command); }

Command::~Command() { delete this->ast; }

void Command::tokenize(std::string command) {
    std::vector<std::string> tokens = split_string(command, " ");

    // Transforms the 3 first tokens in upper case strings.
    // The fourth one and after are untouched since it can also be part of the
    // label.
    std::transform(
        tokens.begin(), tokens.begin() + std::min<size_t>(tokens.size(), 3),
        tokens.begin(), [](std::string &s) {
            std::transform(s.begin(), s.end(), s.begin(),
                           [](const char &c) { return (char)std::toupper(c); });

            return s;
        });

    this->ast = new Tokens{NOINST, NOPIN, OFF, std::string()};

    if (tokens[0] == "SET") {
        this->ast->instruction = SET;
    } else if (tokens[0] == "SHOW") {
        this->ast->instruction = SHOW;
    } else if (tokens[0] == "TOOGLE") {
        this->ast->instruction = TOGGLE;
    } else if (tokens[0] == "LABEL") {
        this->ast->instruction = LABEL;
    } else {
        throw CommandException("ERROR: Invalid instruction");
    }

    if (tokens.size() == 1) {
        return;
    }

    if (tokens[1] != "PIN") {
        throw CommandException("ERROR: Invalid argument");
    }

    if (tokens.size() == 2) {
        throw CommandException("ERROR: Not enough arguments");
    }

    if (tokens[2] == "ALL") {
        this->ast->pin = ALL;
    } else {
        int pin_nbr;

        try {
            pin_nbr = std::stoi(tokens[2]);
        } catch (...) {
            throw CommandException("ERROR: Invalid pin number '%s'",
                                   tokens[2].c_str());
        }

        if (pin_nbr < 2 || pin_nbr > 9) {
            throw PinException(pin_nbr);
        }

        this->ast->pin = static_cast<Pins>(pin_nbr);
    }

    if (tokens.size() == 3) {
        return;
    }

    if (istrcmp(tokens[3], "HIGH") || istrcmp(tokens[3], "ON")) {
        this->ast->state = ON;
    } else if (istrcmp(tokens[3], "LOW") || istrcmp(tokens[3], "OFF")) {
        this->ast->state = OFF;
    } else {
        std::vector<std::string> label(tokens.begin() + 3, tokens.end());
        this->ast->label = vector_join(label);
    }
}

std::vector<uint8_t> Command::execute(Parallel *parallel) {
    switch (this->ast->instruction) {
    case SET: {
        switch (this->ast->pin) {
        case ALL:
            parallel->set_all(this->ast->state);
            return parallel->as_memory();
        default:
            parallel->set_pin(this->ast->pin, this->ast->state);
            return (*parallel)[this->ast->pin];
        }
    } break;
    case SHOW: {
        switch (this->ast->pin) {
        case ALL:
        case NOPIN:
            return parallel->as_memory();
        default:
            return (*parallel)[this->ast->pin];
        }
    } break;
    case TOGGLE: {
        switch (this->ast->pin) {
        case ALL:
            parallel->toggle_all();
            return parallel->as_memory();
        default:
            parallel->toggle_pin(this->ast->pin);
            return (*parallel)[this->ast->pin];
        }
    } break;
    case LABEL: {
        switch (this->ast->pin) {
        case ALL:
            throw PinException();
        default:
            parallel->set_label(this->ast->pin, this->ast->label);
            return (*parallel)[this->ast->pin];
        }
    } break;
    default:
        throw CommandException("ERROR: Invalid instruction");
    }
}
