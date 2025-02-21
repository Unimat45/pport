import Socket from "./Socket";

import "./globals.css";

/* Elements */
const alert_modal = document.getElementById("alert");
const btnDark = document.getElementById("btn-dark");

/* WebSocket */
export const ws = new Socket(`ws://${location.host}:5663`);

ws.addEventListener("error", () => {
	alert_modal?.classList.add("show");
});

ws.addEventListener("close", () => {
	alert_modal?.classList.add("show");
});

ws.addEventListener("open", () => {
	ws.show();
});

if (btnDark != null) {
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
}
