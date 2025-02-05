import { ws } from "./global";

import "./styles.css";

const switches = document.querySelectorAll(".switch");
const pins = Array.from(document.querySelectorAll(".pin"));

ws.onPinMessage((parallel, error) => {
	if (error != null) {
		return alert(error);
	}

	for (let i = 0; i < 8; i++) {
		const lbl = pins[i].querySelector(".label")!;
		lbl.textContent = parallel[i].label;
		const _switch = pins[i].querySelector(".switch")! as HTMLElement;
		if (parallel[i].state) {
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
