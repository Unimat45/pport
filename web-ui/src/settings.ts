import Socket from "./Socket.js";

(() => {
	const btnDark = document.getElementById("btn-dark")!;
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

const ws = new Socket("ws://localhost:5663");

const STATES = ["Éteint", "Allumé"] as const;

const state = document.getElementById("state");
const alert_modal = document.getElementById("alert");
const add_timing = document.querySelector(".add-timing");
const timings_wrapper = document.querySelector(".timings");
const title = document.getElementById("title") as HTMLElement;
const save_btn = document.querySelector(".save-timing") as HTMLButtonElement;
const timing_template = document.getElementById("tm-temp") as HTMLTemplateElement;

const pin = parseInt(new URLSearchParams(location.search).get("pin") || "2") - 2;
const pad = (n: number) => new String(n).padStart(2, "0");

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
		alert(error);
	} else if (state != null) {
		const p = parallel[pin];

		title.querySelector("span")!.textContent = p.label;
		state.classList.toggle("click", p.state);
		state.textContent = STATES[Number(p.state)];

		timings_wrapper?.querySelectorAll(".timing").forEach((e) => e.remove());

		for (const t of p.timings) {
			const temp_clone = timing_template.content.cloneNode(true) as HTMLDivElement;

			const m_btns = Array.from(temp_clone.querySelectorAll(".months button")) as HTMLElement[];

			for (let i = 0; i < 12; i++) {
				if (t.months & (1 << i)) {
					m_btns[i].classList.add("on");
				}
			}

			(temp_clone.querySelector(".state") as HTMLInputElement).value = t.state ? "1" : "0";
			(temp_clone.querySelector(".time") as HTMLInputElement).value = `${pad(t.hour)}:${pad(t.min)}`;

			temp_clone.querySelector(".delete")?.addEventListener("click", (e) => {
				const target = e.target as HTMLElement;
				target.parentElement?.remove();
			});

			addButtonsEvent(m_btns);

			timings_wrapper?.insertBefore(temp_clone, add_timing);
		}
	}
});

title.addEventListener("click", () => {
	const span = document.querySelector("span")!;
	const input = document.querySelector("input")!;

	const v = span.textContent;
	input.value = v || "";
	title.dataset.edit = "true";
});

title.querySelector("input")?.addEventListener("blur", () => {
	const input = document.querySelector("input")!;

	const v = input.value;
	title.dataset.edit = "false";

	ws.setLabel(pin, v);
});

save_btn.addEventListener("click", () => {
	const timings = document.querySelectorAll(".timing");
	ws.removeTimings(pin);

	for (const t of timings) {
		let m = 0;
		t.querySelectorAll(".months button").forEach((b, i) => {
			m |= Number(b.classList.contains("on")) << i;
		});

		const [hh, mm] = (t.querySelector(".time") as HTMLInputElement).value.split(":").map((n) => parseInt(n));

		ws.addTiming(pin, {
			months: ((m & 0xff) << 8) | ((m >> 8) & 0xff),
			hour: hh,
			min: mm,
			state: (t.querySelector(".state") as HTMLSelectElement).value === "1",
		});
	}
});

state?.addEventListener("click", () => {
	const isOn = state.classList.contains("click");
	ws.setPin(pin, !isOn);
});

add_timing?.addEventListener("click", () => {
	const temp_clone = timing_template.content.cloneNode(true) as HTMLDivElement;

	const m_btns = Array.from(temp_clone.querySelectorAll(".months button")) as HTMLElement[];

	addButtonsEvent(m_btns);

	temp_clone.querySelector(".delete")?.addEventListener("click", (e) => {
		const target = e.target as HTMLElement;
		target.parentElement?.remove();
	});

	timings_wrapper?.insertBefore(temp_clone, add_timing);
});

function addButtonsEvent(btns: HTMLElement[]) {
	const findFirstOn = () => btns.findIndex((e) => e.classList.contains("on"));
	const findLastOn = () => btns.findLastIndex((e) => e.classList.contains("on"));

	btns.forEach((btn, i) => {
		btn.addEventListener("click", () => {
			const isOn = btn.classList.contains("on");

			const s = findFirstOn();
			const e = findLastOn();

			// To Toggle On
			if (!isOn) {
				btn.classList.add("on");

				if (s > i) {
					for (let j = i + 1; j < s; j++) {
						btns[j].classList.add("on");
					}
				} else if (s !== -1) {
					for (let j = s + 1; j < i; j++) {
						btns[j].classList.add("on");
					}
				}
			}
			// To Toggle Off
			else {
				btn.classList.remove("on");

				if (e !== -1 && s !== i) {
					for (let j = i + 1; j <= e; j++) {
						btns[j].classList.remove("on");
					}
				}
			}

			btn.blur();
		});
	});
}
