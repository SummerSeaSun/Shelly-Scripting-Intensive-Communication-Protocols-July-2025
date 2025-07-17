const BLU_EVENT_NAME = "shelly-blu";

const DEVICES = ["7c:c6:b6:09:94:21", "e8:e0:7e:bf:21:39"];
const SKIP_FIELDS = ["encryption", "BTHome_version", "pid", 
                     "rssi", "address", "model",
                     "battery"];

Shelly.addEventHandler(function(event){
  if (event.info.event === BLU_EVENT_NAME) {
    let data = event.info.data;
    if (DEVICES.indexOf(data.address) !== -1) {
      for (let field of SKIP_FIELDS) {
        delete data[field];
      }
      console.log(JSON.stringify(data));
    }
  }
});