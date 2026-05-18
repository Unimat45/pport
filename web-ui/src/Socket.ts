export type Timing = { state: boolean; range: number; hour: number; min: number };
type Pin = { state: boolean; label: string; timings: Timing[] };
type Parallel = { pins: Pin[]; enabled_pins: number; isPinEnabled: (pin: number) => boolean };

interface ParallelEventMap extends WebSocketEventMap {
    "parallel": CustomEvent<{ parallel: Parallel }>;
    "parallelError": CustomEvent<string>
}

const enum Action {
    NotSet = 0,
    Show,
    Set,
    Toggle,
    Label,
    Timings,
    DeleteTiming,
}

function strlen(buf: Uint8Array) {
    let len = 0;
    while (buf.length > len && buf[len++] != 0) {}
    return len - 1;
}

class Socket extends WebSocket {
    constructor(url: string | URL, protocols?: string | string[] | undefined) {
        super(url, protocols);

        this.binaryType = "arraybuffer";

        this.addEventListener("message", async (ev) => {
            const data = ev.data as string | ArrayBuffer;

            const parallel: Parallel = {
                pins: [],
                enabled_pins: 0xff,
                isPinEnabled(pin) {
                    return ((1 << pin) & this.enabled_pins) != 0;
                },
            };

            if (typeof data === "string") {
                const errorEvent = new CustomEvent("parallelError", {
                    cancelable: true,
                    detail: data,
                });

                this.dispatchEvent(errorEvent);

                return;
            }

            const buf = new Uint8Array(data);


            let buf_i = 0;

            const version = buf[0];

            if (version > 1) {
                buf_i++;
                parallel.enabled_pins = buf[buf_i++];
            }

            for (let i = 0; i < 8; i++) {
                const lbl_len = strlen(buf.slice(buf_i + 1));

                const p: Pin = {
                    state: buf[buf_i] === 1,
                    label: String.fromCodePoint(...buf.slice(buf_i + 1, buf_i + 1 + lbl_len)),
                    timings: [],
                };

                let base = buf_i + 2 + lbl_len;
                while (buf[base] !== 0 || buf[base + 1] !== 0) {
                    p.timings.push({
                        range: (buf[base] << 24) | (buf[base + 1] << 16) | (buf[base + 2] << 8) | buf[base + 3],
                        hour: buf[base + 4],
                        min: buf[base + 5],
                        state: buf[base + 6] === 1,
                    });

                    base += 7;
                }

                parallel.pins.push(p);

                buf_i = 2 + base;
            }

            const messageEvent = new CustomEvent("parallel", {
                cancelable: true,
                detail: { parallel }
            });

            this.dispatchEvent(messageEvent);
        });
    }

    override addEventListener<K extends keyof ParallelEventMap>(type: K, listener: (this: WebSocket, ev: ParallelEventMap[K]) => any, options?: boolean | AddEventListenerOptions): void {
        // @ts-expect-error overload
        super.addEventListener(type, listener, options);
    }

    override removeEventListener<K extends keyof ParallelEventMap>(type: K, listener: (this: WebSocket, ev: ParallelEventMap[K]) => any, options?: boolean | AddEventListenerOptions): void {
        // @ts-expect-error overload
        super.removeEventListener(type, listener, options);
    }

    public show() {
        this.send(new Uint8Array([Action.Show]));
    }

    public setPin(pin: number, state: boolean) {
        this.send(new Uint8Array([Action.Set, pin + 2, Number(state)]));
    }

    public setLabel(pin: number, label: string) {
        this.send(new Uint8Array([Action.Label, pin + 2, ...label.split("").map((c) => c.charCodeAt(0)), 0]));
    }

    public addTiming(pin: number, timing: Timing) {
        this.send(
            new Uint8Array([
                Action.Timings,
                pin + 2,
                (timing.range >> 24) & 0xff,
                (timing.range >> 16) & 0xff,
                (timing.range >> 8) & 0xff,
                timing.range & 0xff,
                timing.hour,
                timing.min,
                Number(timing.state),
            ]),
        );
    }

    public removeTimings(pin: number) {
        this.send(new Uint8Array([Action.DeleteTiming, pin + 2]));
    }
}

export default Socket;
