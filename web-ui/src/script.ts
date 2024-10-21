import Socket from "./Socket.js";

const btnDark = document.getElementById("btn-dark")!;
const alert_modal = document.getElementById("alert");
const switches = document.querySelectorAll(".switch");
const pins = Array.from(document.querySelectorAll(".pin"));
const ws = new Socket("ws://localhost:5663");

ws.addEventListener("error", () => {
	alert_modal?.classList.add("show");
});

ws.addEventListener("close", () => {
	alert_modal?.classList.add("show");
});

ws.addEventListener("open", () => {
	ws.show();
});

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

(() => {
	const isDark = localStorage.getItem("dark") === "true";
	if (isDark) {
		document.body.classList.add("dark");
		btnDark.classList.add("play");
	}

	btnDark.addEventListener("click", () => {
		btnDark.classList.add("play");
		document.body.classList.toggle("dark");
		localStorage.setItem("dark", document.body.classList.contains("dark").toString());
	});

	btnDark.addEventListener("animationiteration", () => {
		btnDark.classList.remove("play");
	});
})();

switches.forEach((s, i) => {
	s.addEventListener("click", () => {
		const state = s.hasAttribute("data-on");
		ws.setPin(i, !state);
	});
});
