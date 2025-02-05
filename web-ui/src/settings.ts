import { ws } from "./global";

import "./settings.css";

const STATES = ["Éteint", "Allumé"] as const;

/* Elements */
const state = document.getElementById("state");
const add_timing = document.querySelector(".add-timing");
const timings_wrapper = document.querySelector(".timings");
const title = document.getElementById("title") as HTMLElement;
const save_btn = document.querySelector(".save-timing") as HTMLButtonElement;
const timing_template = document.getElementById("tm-temp") as HTMLTemplateElement;

/* Helpers */
const pin = parseInt(new URLSearchParams(location.search).get("pin") || "2") - 2;
const pad = (n: number) => new String(n).padStart(2, "0");

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

            const day1 = temp_clone.querySelector(".day1") as HTMLSelectElement;
            const month1 = temp_clone.querySelector(".month1") as HTMLSelectElement;
            const day2 = temp_clone.querySelector(".day2") as HTMLSelectElement;
            const month2 = temp_clone.querySelector(".month2") as HTMLSelectElement;

            month1.addEventListener("change", () => {
                loadDaysInMonth(parseInt(month1.value), day1);
            });
        
            month2.addEventListener("change", () => {
                loadDaysInMonth(parseInt(month2.value), day2);
            });
        
            loadDaysInMonth(1, day1);
            loadDaysInMonth(1, day2);

            day1.value = ((t.range >> 24) & 0xFF) + "";
            month1.value = ((t.range >> 16) & 0xFF) + "";
            day2.value = ((t.range >> 8) & 0xFF) + "";
            month2.value = (t.range & 0xFF) + "";
            
			(temp_clone.querySelector(".state") as HTMLInputElement).value = t.state ? "1" : "0";
			(temp_clone.querySelector(".time") as HTMLInputElement).value = `${pad(t.hour)}:${pad(t.min)}`;
			temp_clone.querySelector(".delete")?.addEventListener("click", (e) => {
				const target = e.target as HTMLElement;
				target.parentElement?.remove();
			});
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
		const day1 = t.querySelector(".day1") as HTMLSelectElement;
		const month1 = t.querySelector(".month1") as HTMLSelectElement;
		const day2 = t.querySelector(".day2") as HTMLSelectElement;
		const month2 = t.querySelector(".month2") as HTMLSelectElement;

		const range = ((parseInt(day1.value) & 0xff) << 24) | ((parseInt(month1.value) & 0xff) << 16) | ((parseInt(day2.value) & 0xff) << 8) | (parseInt(month2.value) & 0xff);

		const [hh, mm] = (t.querySelector(".time") as HTMLInputElement).value.split(":").map((n) => parseInt(n));

		ws.addTiming(pin, {
			range,
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

	temp_clone.querySelector(".delete")?.addEventListener("click", (e) => {
		const target = e.target as HTMLElement;
		target.parentElement?.remove();
	});

	const month1 = temp_clone.querySelector(".month1") as HTMLSelectElement;
	const month2 = temp_clone.querySelector(".month2") as HTMLSelectElement;
	const date1 = temp_clone.querySelector(".day1") as HTMLSelectElement;
	const date2 = temp_clone.querySelector(".day2") as HTMLSelectElement;

	month1.addEventListener("change", () => {
		loadDaysInMonth(parseInt(month1.value), date1);
	});

	month2.addEventListener("change", () => {
		loadDaysInMonth(parseInt(month2.value), date2);
	});

	loadDaysInMonth(1, date1);
	loadDaysInMonth(1, date2);

	timings_wrapper?.insertBefore(temp_clone, add_timing);
});

function loadDaysInMonth(month: number, recipent: HTMLSelectElement) {
	const dt = new Date(2000, month, 0);

	[...recipent.children].forEach((e) => e.remove());

	for (let i = 1; i <= dt.getDate(); i++) {
		const opt = document.createElement("option");

		opt.value = i + "";
		opt.innerText = i + "";

		recipent.append(opt);
	}
}
