const LORA_BLU_EVENT_NAME = "LORA-BLU"
function processLoRaBLUData(data) {
    console.log("Emitting event with data:", data);
    try {
        let obj = JSON.parse(data)
        Shelly.emitEvent(LORA_BLU_EVENT_NAME, obj);
    } catch (error) {
        console.log("Error processing data");
    }

}

Shelly.addEventHandler(function (event) {
    if (event.component === "lora:100" && event.info.event === "lora_received") {
        let data = atob(event.info.data);
        // check for BLU data
        if (data.substr(0, 5) !== "<BLU>") {
            console.log("The received data is not a BLU event");
            return;
        }
        let message = data.substr(5);
        processLoRaBLUData(message);
    }
});