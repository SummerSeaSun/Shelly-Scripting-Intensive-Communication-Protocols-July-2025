const CONFIG = {
    ME: "main_controller",
    ALERT_TIMER_PERIOD: 10000,
    DANGER_TIMER_PERIOD: 10000,
    TEMPERATURE_THRESHOLD: 25,

    //MQTT prefixes
    MQTT_PREFIX_GH_CONTROL: "green_house_controller",
    MQTT_PREFIX_LED: "control_center_led",
    //VC ids
    // synchronized from remote
    VC_FAN1_STATE: "boolean:200",
    VC_FAN2_STATE: "boolean:201",
    VC_TEMPERATURE: "number:200",

    // locally managed
    VC_GH_STATUS: "enum:200",

    //TEMP_COMPONENT: "temperature:100",
    //TEMP_FIELD: "tC",
    TEMP_COMPONENT: "number:200",
    TEMP_FIELD: "value",
};

let vcFan1Handler = Virtual.getHandle(CONFIG.VC_FAN1_STATE);
let vcFan2Handler = Virtual.getHandle(CONFIG.VC_FAN2_STATE);
let vcTemperatureHandler = Virtual.getHandle(CONFIG.VC_TEMPERATURE);
let vcGHStatusHandler = Virtual.getHandle(CONFIG.VC_GH_STATUS);

let mqttLedTopic = CONFIG.MQTT_PREFIX_LED + "/rpc";
let mqttGHControlTopic = CONFIG.MQTT_PREFIX_GH_CONTROL + "/rpc";
let mqttGHEventsTopic = CONFIG.MQTT_PREFIX_GH_CONTROL + "/events/rpc";
let mqttResponseTopic = CONFIG.ME + "/rpc";

let alertTimerHandler;
let dangerTimerHandler;

let temperatureRequestId;
let fan1RequestId;
let fan2RequestId;

let rpc_id = 1;

//send rpc over mqtt
function sendRPC(topic, method, params) {
    let rpc = {
        id: rpc_id++,
        src: CONFIG.ME,
        method: method,
        params: params
    }

    MQTT.publish(topic, JSON.stringify(rpc));
}

function changeLED(rgb) {
    let params = {
        id: 0,
        on: true,
        rgb: rgb,
    }
    sendRPC(mqttLedTopic, "RGB.Set", params);
}

function changeStatus(newStatus) {
    if (vcGHStatusHandler.getValue() === newStatus) return;
    console.log("changing status to:", newStatus);
    vcGHStatusHandler.setValue(newStatus);
    let rgb;
    if (newStatus === "normal") {
        sendRPC(mqttGHControlTopic, "Switch.Set", { id: 0, on: false });
        sendRPC(mqttGHControlTopic, "Switch.Set", { id: 1, on: false });
        rgb = [0, 255, 0];
    } else if (newStatus === "warning") {
        sendRPC(mqttGHControlTopic, "Switch.Set", { id: 0, on: true });
        rgb = [255, 255, 0];
    } else if (newStatus === "alert") {
        sendRPC(mqttGHControlTopic, "Switch.Set", { id: 1, on: true });
        rgb = [255, 100, 0];
    } else if (newStatus === "danger") {
        rgb = [255, 0, 0];
        // send email
    }
    changeLED(rgb);
}

function dangerTimer() {
    changeStatus("danger");
}

function alertTimer() {
    // first time period is over and we are still above threshold temprature
    changeStatus("alert");
    alertTimerHandler = undefined;
    dangerTimerHandler = Timer.set(CONFIG.DANGER_TIMER_PERIOD, false, dangerTimer);
}

function handleTemperatureChanges(event) {
    if (CONFIG.TEMP_COMPONENT in event) {
        let temp = event[CONFIG.TEMP_COMPONENT][CONFIG.TEMP_FIELD];
        console.log("New temperature:", temp);

        if (temp >= CONFIG.TEMPERATURE_THRESHOLD &&
            alertTimerHandler === undefined &&
            dangerTimerHandler === undefined) {
            changeStatus("warning");
            alertTimerHandler = Timer.set(CONFIG.ALERT_TIMER_PERIOD, false, alertTimer);
        } else if (temp < CONFIG.TEMPERATURE_THRESHOLD) {
            changeStatus("normal");
            if (alertTimerHandler !== undefined) {
                Timer.clear(alertTimerHandler);
                alertTimerHandler = undefined;
            }
            if (dangerTimerHandler !== undefined) {
                Timer.clear(dangerTimerHandler);
                dangerTimerHandler = undefined;
            }
        }
    }
}

function handleRemoteStates(event) {
    //update fan1 state from remote switch:0 state
    if ("switch:0" in event && typeof event["switch:0"].output != "undefined") {
        vcFan1Handler.setValue(event["switch:0"].output);
    }
    //update fan2 state from remote switch:1 state
    if ("switch:1" in event && typeof event["switch:1"].output != "undefined") {
        vcFan2Handler.setValue(event["switch:1"].output);
    }
    //update temperature from remote temperature reading
    if (CONFIG.TEMP_COMPONENT in event) {
        let temp = event[CONFIG.TEMP_COMPONENT][CONFIG.TEMP_FIELD];
        vcTemperatureHandler.setValue(temp);
    }
}

function readRemoteState() {
    // read temperature from remote
    temperatureRequestId = rpc_id;
    sendRPC(mqttGHControlTopic,
        CONFIG.TEMP_COMPONENT.split(":")[0] + ".GetStatus",
        {
            id: Number(CONFIG.TEMP_COMPONENT.split(":")[1])
        });
    fan1RequestId = rpc_id;
    sendRPC(mqttGHControlTopic, "Switch.GetStatus", { id: 0 });
    fan2RequestId = rpc_id;
    sendRPC(mqttGHControlTopic, "Switch.GetStatus", { id: 1 });
}

MQTT.subscribe(mqttResponseTopic, function (topic, data) {
    let event = JSON.parse(data);
    if (event.id === temperatureRequestId) {
        vcTemperatureHandler.setValue(event.result[CONFIG.TEMP_FIELD]);
    } else if (event.id === fan1RequestId) {
        vcFan1Handler.setValue(event.result.output);
    } else if (event.id === fan2RequestId) {
        vcFan2Handler.setValue(event.result.output);
    }
});

MQTT.subscribe(mqttGHEventsTopic, function (topic, data) {
    let event = JSON.parse(data);
    if (event.method === "NotifyStatus") {
        handleTemperatureChanges(event.params);
        handleRemoteStates(event.params);
    }
});

readRemoteState();