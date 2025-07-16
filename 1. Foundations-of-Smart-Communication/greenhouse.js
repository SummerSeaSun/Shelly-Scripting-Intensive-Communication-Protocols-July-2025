const CONFIG = {
    ALERT_TIMER_PERIOD: 10000,
    DANGER_TIMER_PERIOD: 10000,
    TEMPERATURE_THRESHOLD: 25,
    //  TEMP_COMPONENT: "temperature:100",
    //  TEMP_FIELD: "tC",
    TEMP_COMPONENT: "number:200",
    TEMP_FIELD: "value",
    VC_STATUS: "enum:200",
    RGB_DEVICE_ID: "b0a732409acc",
    CONTROL_API_SERVER: "https://shelly-97-eu.shelly.cloud",
    AUTH_KEY: "{auth_key}",
    BREVO_API_KEY: "{api-key}"
};

let vcStatusHandler = Virtual.getHandle(CONFIG.VC_STATUS);

let alertTimerHandler;
let dangerTimerHandler;

function changeLED(rgb) {
    let url = CONFIG.CONTROL_API_SERVER + "/v2/devices/api/set/light?auth_key=" + CONFIG.AUTH_KEY;
    let body = {
        id: CONFIG.RGB_DEVICE_ID,
        on: true,
        red: rgb[0],
        green: rgb[1],
        blue: rgb[2]
    }
    Shelly.call("HTTP.POST", { url: url, body: body });
}

function changeStatus(newStatus) {
    if (vcStatusHandler.getValue() === newStatus) return;
    console.log("changing status to:", newStatus);
    vcStatusHandler.setValue(newStatus);
    let rgb;
    if (newStatus === "normal") {
        Shelly.call("Switch.Set", { id: 0, on: false });
        Shelly.call("Switch.Set", { id: 1, on: false });
        rgb = [0, 255, 0];
    } else if (newStatus === "warning") {
        Shelly.call("Switch.Set", { id: 0, on: true });
        rgb = [255, 255, 0];
    } else if (newStatus === "alert") {
        Shelly.call("Switch.Set", { id: 1, on: true });
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
    if (event.component === CONFIG.TEMP_COMPONENT && event.delta[CONFIG.TEMP_FIELD]) {
        let temp = event.delta[CONFIG.TEMP_FIELD];
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

Shelly.addStatusHandler(handleTemperatureChanges);

  //changeLED([255,0,0]);