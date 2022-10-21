#include "Command.h"

#include <regex>

#define NO_PIN -1
#define ALL_PINS 0xFF

#define DEFAULT_LABEL(pin) "Pin " + std::to_string(pin)

const std::regex RE_CMD("^(SET|SHOW(?:STR)?|REBOOT|TOGGLE)(?: (?:PIN ([2-9]|ALL))(?: (LABEL|HIGH|LOW) ?)?(\\w+)?)?$", std::regex_constants::icase);

Command::Command(std::string cmd) {
    this->cmd = cmd;
    
    std::smatch match;

    if (!std::regex_search(this->cmd, match, RE_CMD)) {
        throw std::runtime_error("ERROR: There is an error in your syntax");
    }

    this->inst = toUpper(match[1].str()); // Sets instruction


    this->pin = match[2].str() != "" ? ( toUpper(match[2].str()) == "ALL" ? ALL_PINS : std::stoi(match[2].str()) ) : NO_PIN;

    if (toUpper(match[3].str()) == "LABEL") {
        this->inst = "LABEL";
    }

    this->state = match[3].str() != "" && toUpper(match[3].str()) == "HIGH";

    this->label = toUpper(match[4].str()) == "DEFAULT" ? DEFAULT_LABEL(this->pin) : match[4].str();
}

std::string Command::Execute() {
    if (this->inst == "SET") {

        // If PIN is ALL
        if (this->pin == ALL_PINS) {
            for (size_t i = 0; i < PORT_SIZE; i++) {
                Pin* p = parallel[i + 2];

                p->SetState(this->state);
            }

            parallel.WriteToFile();

            return parallel.ToJSONStr();
        }
        
        Pin* p = parallel[this->pin];

        p->SetState(this->state);

        parallel.WriteToFile();

        return p->ToJSONStr();
    }

    else if (this->inst == "SHOW") {
        // If only SHOW or SHOW ALL
        if (this->pin == NO_PIN || this->pin == ALL_PINS) {
            return parallel.ToJSONStr();
        }

        Pin* p = parallel[this->pin];

        return p->ToJSONStr();
    }

    else if(this->inst == "SHOWSTR") {
        // If only SHOW or SHOW ALL
        if (this->pin == NO_PIN || this->pin == ALL_PINS) {
            return parallel.ToString();
        }

        Pin* p = parallel[this->pin];

        return p->ToString();
    }
    
    else if (this->inst == "TOGGLE") {

        // If PIN is ALL
        if (this->pin == ALL_PINS) {
            for (size_t i = 0; i < PORT_SIZE; i++) {
                Pin* p = parallel[i + 2];

                p->ToggleState();
            }

            parallel.WriteToFile();

            return parallel.ToJSONStr();
        }
        
        Pin* p = parallel[this->pin];

        p->ToggleState();

        parallel.WriteToFile();

        return p->ToJSONStr();
    }

    else if (this->inst == "LABEL") {
        // If no pin specified, throw error
        if (this->pin == NO_PIN) throw std::runtime_error("ERROR: No pin was set for the label (Needed: ALL or pin number)");

        // If ALL pins specified
        else if (this->pin == ALL_PINS) {
            
            for (size_t i = 0; i < PORT_SIZE; i++) {
                Pin* p = parallel[i + 2];

                p->SetLabel(this->label);
            }

            parallel.WriteToFile();

            return parallel.ToJSONStr();
        }
        
        Pin* p = parallel[this->pin];

        p->SetLabel(this->label);

        parallel.WriteToFile();

        return p->ToJSONStr();
    }
    
    else if (this->inst == "REBOOT") {
        parallel = Parallel(); // Reset from file

        return parallel.ToJSONStr();
    }

    throw std::runtime_error("ERROR: There is an error in the instruction");
}