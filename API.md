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
* Timings: 4 bytes representing the start day, start month, end day, end month in a big endian layout, followed by the hour, minutes, and state, each being 1 byte
* DeleteTimings: No payload
* DeleteTiming: 2 bytes representing the months in a big endian layout, followed by the hour, minutes, and state, each being 1 byte

The range is represented by a 32 bit number, where the 2 first bytes are the start of the range and the last 2 are the end of the range.
January 15th can be represented by the number `0x0F01`, and December 31st by `0x1F0C`. So, the complete range would like `0x0F011F0C`.

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
