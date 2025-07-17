const BLU_EVENT_NAME = "shelly-blu";

const DEVICES = ["7c:c6:b6:09:94:21", "e8:e0:7e:bf:21:39", "7c:c6:b6:73:bb:e2"];
const SKIP_FIELDS = ["encryption", "BTHome_version", "pid",
    "rssi", "battery"];
const MSG_PREFIX = "<BLU>";

//an example for more compact transfer will be to map field names to numbers
//and use the same mapping on the other side to reverse the names
/*const FIELD_MAP={
  "address": 0,
  "model": 1,
  "temperature": 2,
  "humidity": 3
}*/

// no ecryption - can be easily added
// no dst id but can be just added to the payload
function sendLoRa(data) {
    let message = MSG_PREFIX + JSON.stringify(data);
    let payload = btoa(message);
    Shelly.call("lora.sendbytes", { id: 100, data: payload });
}

Shelly.addEventHandler(function (event) {
    if (event.info.event === BLU_EVENT_NAME) {
        let data = event.info.data;
        //console.log(data.address);
        if (DEVICES.indexOf(data.address) !== -1) {
            for (let field of SKIP_FIELDS) {
                delete data[field];
            }
            console.log("sending:", JSON.stringify(data));
            sendLoRa(data);
        }
    }
});