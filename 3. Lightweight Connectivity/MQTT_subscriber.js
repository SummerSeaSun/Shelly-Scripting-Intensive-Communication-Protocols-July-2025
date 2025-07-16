MQTT.subscribe("home/devices/control", function (topic, data) {
    if (data === "on") {
        Shelly.call("Switch.Set", { id: 0, on: true });
        Shelly.call("Switch.Set", { id: 1, on: true });
    } else if (data === "off") {
        Shelly.call("Switch.Set", { id: 0, on: false });
        Shelly.call("Switch.Set", { id: 1, on: false });
    }
});


// Script in RGBW device

MQTT.subscribe("home/devices/control", function (topic, data) {
    if (data === "on") {
        Shelly.call("RGB.Set", { id: 0, on: true });
    } else if (data === "off") {
        Shelly.call("RGB.Set", { id: 0, on: false });
    }
});