# PPORT

Control your linux server's parallel port easily with this
fast and intuitive program

Written in C, using this [WebSocket Library](https://github.com/Theldus/wsServer) by Theldus

**Take note that this program only works for linux machines!**

## Links

* [Server API](#serverapi)
* [Web interface](#webinterface)
* [Installation](#installation)
* [TODO](#todo)

# Server API	
The communication with the server is done through binary array messages, following a specific format documented [here](./API.md)

### Examples

    // Show port state
    uint8_t message = { 1 };

    // Set the first pin's state as high
    uint8_t message = { 2, 2, 1 };

    // Set the third pin's label as "Hello"
    uint8_t message = { 4, 5, 'H', 'e', 'l', 'l', 'o', 0 };

# Web Interface
The web interface is written in vanilla html and css, but is using TypeScript for
the scripts, that need to be compiled to run. See [Installation](#installation) to do so.

There are 2 pages available: index and settings.

Index page is prety simple, 8 switches. Just click on a switch to toggle it

The settings page is a bit more complicated. The top is the simpler part, it is pretty much the same as the index page, but you can change the pin label by clicking it. It will save once you click away from the input.

The timings section is where things become weird. You can add a timing by click the + button. In this new timing, you can specify which months you want it to execute, the time it should execute and whether you want to turn on or off this pin. **No changes will be made unless you click the `Save` button.**

# Installation
Download the source code and build it using CMake. There is a custom flag to change the config file directory.

    git submodule init
	cmake -B build -D CFG_FILE=/etc/pport.cfg .
    make -j$(nproc) -C build
    sudo make install

# TODO

1. Fix the months selection to accept month gaps in the same timing
2. Better handling of the label input
3. Better handling of the timing loop
4. Add compile flag for WebSocket port