const timezone_offset_seconds = new Date().getTimezoneOffset() * 60;

const hex_per_day = 16 ** 4;
const sec_per_day = 24 * 60 * 60;




const id = setInterval(function () {
    const seconds =
        (Date.now() / 1000 - timezone_offset_seconds) % (24 * 60 * 60);
    const hex_seconds = (seconds / sec_per_day) * hex_per_day;
    const hex_string = Math.trunc(hex_seconds)
        .toString(16)
        .padStart(4, "0");
    document.getElementById("hex_now").innerText = hex_string;
}, 100);

document.getElementById("hex").onchange = function () {
    const hex_seconds = parseInt(this.value, 16);
    const seconds = (hex_seconds / hex_per_day) * sec_per_day;

    const second = Math.trunc(seconds % 60);
    const minute = Math.trunc((seconds / 60) % 60);
    const hour = Math.trunc((seconds / 60 / 60) % 60);

    const s = String(second).padStart(2, 0);
    const m = String(minute).padStart(2, 0);
    const h = String(hour).padStart(2, 0);

    document.getElementById("legacy").innerText = h + ":" + m + ":" + s;
};

let handler = function () {
    const h = parseInt(document.getElementById("hour").value);
    const m = parseInt(document.getElementById("minute").value);
    const s = parseInt(document.getElementById("second").value);

    const seconds = h * 60 * 60 + m * 60 + s;
    const hex_seconds = (seconds / sec_per_day) * hex_per_day;
    const hex_string = Math.trunc(hex_seconds)
        .toString(16)
        .padStart(4, "0");
    document.getElementById("hex_out").innerText = hex_string;
};

document.getElementById("hour").onchange = handler;
document.getElementById("minute").onchange = handler;
document.getElementById("second").onchange = handler;