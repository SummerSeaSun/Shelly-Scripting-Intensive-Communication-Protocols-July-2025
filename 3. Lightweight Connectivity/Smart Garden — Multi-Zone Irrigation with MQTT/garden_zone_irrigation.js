const CONFIG = {
    MOISTURE_THRESHOLD: 40,
    MOISTURE_COMPONENT: "number:201",
    MOISTURE_FIELD: "value",
    MQTT_TRIGGER_TOPIC: "garden/irrigation/trigger"
}

let pumpsOn = false;

Shelly.addStatusHandler(function (event) {
    if (event.component === CONFIG.MOISTURE_COMPONENT && typeof event.delta[CONFIG.MOISTURE_FIELD] != "undefined") {
        let moisture = event.delta[CONFIG.MOISTURE_FIELD];
        if (moisture < CONFIG.MOISTURE_THRESHOLD) {
            if (!pumpsOn) {
                // turn on the pumps
                MQTT.publish(CONFIG.MQTT_TRIGGER_TOPIC, "on");
                pumpsOn = true;
            }
        } else {
            if (pumpsOn) {
                MQTT.publish(CONFIG.MQTT_TRIGGER_TOPIC, "off");
                pumpsOn = false;
            }
        }
    }
});