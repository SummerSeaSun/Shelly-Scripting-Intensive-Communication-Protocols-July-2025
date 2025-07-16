let VC_MSG = "text:200";
let msgHandler = Virtual.getHandle(VC_MSG);

MQTT.subscribe("hello/world", function (topic, data) {
    msgHandler.setValue(data);
});