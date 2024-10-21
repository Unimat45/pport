# Server API
This API is largely inspired by the [Minecraft Query Protocol](https://wiki.vg/Query), though much less secure. It uses bytes to represent actions and data is barely sanitized.

The messages need to be formatted this way:

    { Action, Pin Number, Payload }

The payload varies depending on the action chosen,

There are 7 actions available, representable by an enum

    enum Action {
        // Shows the port's state
        Show = 1,

        // Sets a pin state high or low
        Set,

        // Toggles a pin's state
        Toggle,

        // Set a label to a pin
        Label,

        // Add a timing to a pin
        Timings,

        // Delete all timings associated to a pin
        DeleteTimings,

        // Delete a specific timing associated to a pin
        DeleteTiming,
    };

These actions all require a pin number, going from 2 to 9 inclusively,
except for `Show`. They also require a certain payload, described here:

* Show: No payload
* Set: `0` for low, and `1` high
* Toggle: No payload
* Label: A string represented as bytes, terminated by a `0` (Ex: "Hello" => { 'H', 'e', 'l', 'l', 'o', 0 })
* Timings: 2 bytes representing the months in a big endian layout, followed by the hour, minutes, and state, each being 1 byte
* DeleteTimings: No payload
* DeleteTiming: 2 bytes representing the months in a big endian layout, followed by the hour, minutes, and state, each being 1 byte

The months are represented by a 16 bit number, where each month is a bit from left to rigth.
January can be represented by the number `0b1`, and December by `0b100000000000`. So combining multiple
months, like April to July, is represented this way `0b00000 01111000`, or `{ 0, 0x78 }`.

If an error has occured during the processing of an action, a string message is returned describing the error,
otherwise, a serialized version of the port is send as a binary message under this format:

    {
        Pin state,
        Zero-terminated label,
        timings (if any),
        double zero terminator
    }

A real world example could be

    { 0, 50, 69, 6e, 20, 32, 0, 0, 0, 0, 50, 69, 6e, 20, 33, 0, 0, 0, 0, 50, 69, 6e, 20, 34, 0, 0, 0, 0, 50, 69, 6e, 20, 35, 0, 0, 0, 0, 50, 69, 6e, 20, 36, 0, 0, 0, 0, 50, 69, 6e, 20, 37, 0, 0, 0, 0, 50, 69, 6e, 20, 38, 0, 0, 0, 0, 50, 69, 6e, 20, 39, 0, 0, 0 }
