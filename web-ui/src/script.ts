import { ws } from "./global";

import "./styles.css";

const switches = document.querySelectorAll(".switch");
const pins: HTMLDivElement[] = Array.from(document.querySelectorAll(".pin"));

ws.addEventListener("parallelError", ({ detail }) => {
    alert(detail);
})

ws.addEventListener("parallel", ({ detail: { parallel } }) => {
	for (let i = 0; i < 8; i++) {
        pins[i].style.display = parallel.isPinEnabled(i) ? "" : "none";

		const lbl = pins[i].querySelector(".label")!;
		lbl.textContent = parallel.pins[i].label;
		const _switch = pins[i].querySelector(".switch")! as HTMLElement;
		if (parallel.pins[i].state) {
			_switch.setAttribute("data-on", "");
		} else {
			_switch.removeAttribute("data-on");
		}
	}
});

switches.forEach((s, i) => {
	s.addEventListener("click", () => {
		const state = s.hasAttribute("data-on");
		ws.setPin(i, !state);
	});
});
