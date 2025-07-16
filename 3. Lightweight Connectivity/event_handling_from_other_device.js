const TARGET_DEVICE_ID = "shellyplusrgbwpm-b0a732409acc";

let events_topic = TARGET_DEVICE_ID + "/events/rpc";

MQTT.subscribe(events_topic, function (topic, data) {
    let event = JSON.parse(data);
    if (event.method === "NotifyEvent") {
        for (let event_data of event.params.events) {
            if (event_data.component === "input:0" && event_data.event === "single_push") {
                Shelly.call("Switch.Toggle", { id: 0 });
            } else if (event_data.component === "input:1" && event_data.event === "single_push") {
                Shelly.call("Switch.Toggle", { id: 1 });
            }
        }
    }
    console.log(data);
});


// In other device

MQTT.subscribe("shellyplusrgbwpm-b0a732409acc/events/rpc", function (topic, data) {
    console.log(data);
});