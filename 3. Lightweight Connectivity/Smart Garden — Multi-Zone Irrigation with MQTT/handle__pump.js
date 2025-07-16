const MQTT_PUMPS_CONTROL_TOPIC = "garden/irrigation/trigger";

MQTT.subscribe(MQTT_PUMPS_CONTROL_TOPIC, function (topic, data) {
    if (data === "on") {
        Shelly.call("Switch.Set", { id: 0, on: true });
    } else if (data === "off") {
        Shelly.call("Switch.Set", { id: 0, on: false });
    }
});