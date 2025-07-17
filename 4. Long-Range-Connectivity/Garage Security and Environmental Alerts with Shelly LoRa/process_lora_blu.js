const LORA_BLU_EVENT_NAME = "LORA-BLU";

const HT_ADDR = "7c:c6:b6:09:94:21";
const DOOR_ADDR = "e8:e0:7e:bf:21:39";

let VC_TEMPERATURE = "number:200";
let VC_HUMIDITY = "number:201";
let vcTemperatureHandle = Virtual.getHandle(VC_TEMPERATURE);
let vcHumidityHandle = Virtual.getHandle(VC_HUMIDITY);

Shelly.addEventHandler(function (event) {
    if (event.info.event === LORA_BLU_EVENT_NAME) {
        let data = event.info.data;
        if (data.address === DOOR_ADDR) {
            let isDoorOpened = data.window;
            if (isDoorOpened) {
                let time = Shelly.getComponentStatus("sys").time;
                let hour = Number(time.substr(0, 2));
                if (hour > 22 || hour < 7) {
                    console.log("Door open after hours");
                    // send notification
                }
            }
        } else if (data.address === HT_ADDR) {
            let temperature = data.temperature;
            let humidity = data.humidity;
            vcTemperatureHandle.setValue(temperature);
            vcHumidityHandle.setValue(humidity);

            // optionally save in virtual components to have a dashboard (e.g. control app or grafana)
            if (temperature > 40 || temperature < 0 || humidity > 50) {
                console.log("conditions are not good");
                // send notification
            }
        }

    }
});