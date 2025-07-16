const CONFIG = {
    GH_FAN1_COMPONENT: "switch:0",
    GH_FAN2_COMPONENT: "switch:1",
    GH_STATUS_COMPONENT: "enum:200",
    GH_TEMP_COMPONENT: "number:200",
    FAN1_COMPONENT: "boolean:200",
    FAN2_COMPONENT: "boolean:201",
    STATUS_COMPONENT: "enum:200",
    TEMP_COMPONENT: "number:200",
    DEVICE_ID: "34cdb0773c64",
    CONTROL_API_SERVER: "https://shelly-97-eu.shelly.cloud",
    AUTH_KEY: "{auth_key}",
}

let fan1Handler = Virtual.getHandle(CONFIG.FAN1_COMPONENT);
let fan2Handler = Virtual.getHandle(CONFIG.FAN2_COMPONENT);
let statusHandler = Virtual.getHandle(CONFIG.STATUS_COMPONENT);
let tempHandler = Virtual.getHandle(CONFIG.TEMP_COMPONENT);

function pullData() {
    let url = CONFIG.CONTROL_API_SERVER + "/v2/devices/api/get?auth_key=" + CONFIG.AUTH_KEY;
    let body = {
        "ids": [CONFIG.DEVICE_ID],
        "select": ["status"],
        "pick": {
            "status": [CONFIG.GH_FAN1_COMPONENT, CONFIG.GH_FAN2_COMPONENT,
            CONFIG.GH_STATUS_COMPONENT, CONFIG.GH_TEMP_COMPONENT]
        }
    }
    Shelly.call("HTTP.POST", { url: url, body: body }, function (response) {

        let body = JSON.parse(response.body);
        let fan1 = body[0].status[CONFIG.GH_FAN1_COMPONENT].output;
        let fan2 = body[0].status[CONFIG.GH_FAN2_COMPONENT].output;
        let gh_status = body[0].status[CONFIG.GH_STATUS_COMPONENT].value;
        let temperature = body[0].status[CONFIG.GH_TEMP_COMPONENT].value;

        console.log(fan1, fan2, gh_status, temperature);
        fan1Handler.setValue(fan1);
        fan2Handler.setValue(fan2);
        statusHandler.setValue(gh_status);
        tempHandler.setValue(temperature);
    });
}

Timer.set(10000, true, pullData);