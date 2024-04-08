#include "pio.h"
#include "log.h"
#include "Parallel.h"
#include "PinException.hpp"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <vector>
#include <cstring>
#include <filesystem>

namespace fs = std::filesystem;

#ifndef CFG_FILE
#define CFG_FILE "pport_config.cfg"
#endif

#define PIN_CHECK(n) if (!(n >= 2 && n <= 9)) { throw PinException(n); }

static bool load_from_file(Pin *parallel[PARALLEL_SIZE]) {
    std::ifstream f(CFG_FILE, std::ios::binary | std::ios::ate);
    std::ifstream::pos_type pos = f.tellg();

    if (pos == 0) {
        log_warn("Error loading config file: loading default settings");
        return false;
    }

    std::vector<char> bytes(pos);

    f.seekg(0, std::ios::beg);
    f.read(bytes.data(), pos);

    f.close();

    size_t last_len = 0;

    for (size_t i = 0; i < PARALLEL_SIZE; i++) {
        size_t lbl_len = strnlen(bytes.data() + last_len + 1, 260);

        Pin *p = new Pin {
            static_cast<uint8_t>(bytes.at(last_len)),
            std::string(bytes.data() + last_len + 1, lbl_len)
        };

        parallel[i] = p;
        last_len += lbl_len + 2;
    }

    return true;
}

Parallel::Parallel(void) {
    if (fs::exists(CFG_FILE)) {
        load_from_file(this->parallel);
        io_write(this->bit_value());
    }
    else {
        for (size_t i = 0; i < PARALLEL_SIZE; i++) {
            Pin *p = new Pin;
            p->state = OFF;
            p->label = "Pin ";
            p->label.push_back(static_cast<char>(i) + 2 + '0');

            this->parallel[i] = p;
        }
        this->save_to_cfg();
    }
}

Parallel::~Parallel() {
    this->save_to_cfg();

    for (Pin *p : this->parallel) {
        delete p;
    }
}

uint8_t Parallel::bit_value() {
    uint8_t value = 0;

    for (size_t i = 0; i < PARALLEL_SIZE; i++) {
        Pin *p = this->parallel[i];

        value |= (1 << i) * p->state;
    }

    return value;
}

void Parallel::toggle_pin(size_t pin) {
    PIN_CHECK(pin)
    Pin *p = this->parallel[pin - 2];

    p->state = !p->state;

    io_write(this->bit_value());
}

void Parallel::set_pin(size_t pin, uint8_t value) {
    PIN_CHECK(pin)
    Pin *p = this->parallel[pin - 2];

    p->state = value;

    io_write(this->bit_value());
}

void Parallel::set_label(size_t pin, std::string label) {
    PIN_CHECK(pin)
    Pin *p = this->parallel[pin - 2];

    p->label = label;
}

void Parallel::set_all(uint8_t value) {
    for (Pin *p : this->parallel) {
        p->state = value;
    }

    io_write(this->bit_value());
}

void Parallel::toggle_all() {
    for (Pin *p : this->parallel) {
        p->state = !p->state;
    }

    io_write(this->bit_value());
}

std::vector<uint8_t> Parallel::as_memory() {
    std::vector<uint8_t> mem;
    
    // is array
    mem.push_back(1);

    for (size_t i = 2; i < PARALLEL_SIZE + 2; i++) {
        std::vector<uint8_t> pin_mem = this->operator[](i);

        // begin + 1 for skipping `is array` of the pin mem result
        mem.insert(mem.end(), pin_mem.begin() + 1, pin_mem.end());
    }

    return mem;
}

std::vector<uint8_t> Parallel::operator[](size_t pin) {
    PIN_CHECK(pin)
    Pin *p = this->parallel[pin - 2];

    std::vector<uint8_t> mem;
    mem.reserve(p->label.size() + 2);

    // is array
    mem.push_back(0);
    // state
    mem.push_back(p->state);
    // label
    std::transform(p->label.begin(), p->label.end(), std::back_inserter(mem), [](const char c) {
        return static_cast<uint8_t>(c);
    });
    // string terminator
    mem.push_back(0);

    return mem;
}

void Parallel::save_to_cfg() {
    std::ofstream f(CFG_FILE, std::ios::binary | std::ios::ate);
    std::ostream_iterator<uint8_t> out_it(f);

    std::vector<uint8_t> mem = this->as_memory();
    std::copy(mem.begin() + 1, mem.end(), out_it);

    f.close();
}
