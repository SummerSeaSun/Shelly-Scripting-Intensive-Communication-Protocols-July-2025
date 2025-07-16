const TARGET_DEVICE_ID = "shellyplusrgbwpm-b0a732409acc";
const MY_DEVICE_ID = Shelly.getDeviceInfo().id;
console.log("My device id is", MY_DEVICE_ID);

const RPC_CMD = {
    id: 1,
    src: MY_DEVICE_ID,
    method: "RGB.Set",
    params: {
        id: 0,
        rgb: [255, 0, 0],  //red
        brightness: 100,
        on: true
    }
}

let request_topic = TARGET_DEVICE_ID + "/rpc";
let response_topic = MY_DEVICE_ID + "/rpc";

MQTT.subscribe(response_topic, function (topic, data) {
    console.log("Response from device:", data);
});

let message = JSON.stringify(RPC_CMD);

MQTT.publish(request_topic, message);