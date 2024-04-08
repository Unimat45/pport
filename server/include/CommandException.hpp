#ifndef COMMANDEXCEPTION_H
#define COMMANDEXCEPTION_H

#include <string>
#include <cstdarg>
#include <exception>

class CommandException : public std::exception {
private:
    std::string msg;
public:
    CommandException(const char *msg, ...) {
        va_list args;
        char buf[1024];
        va_start(args, msg);
        vsnprintf(buf, 1024, msg, args);
        va_end(args);

        this->msg = std::string(buf);
    }
    CommandException(std::string msg): msg(msg) {}
    
    const char * what() const noexcept override {
        return msg.c_str();
    }
};

#endif // COMMANDEXCEPTION_H