const devices = ["192.168.10.127", "192.168.10.162", "192.168.10.167", "192.168.10.190"];

function toggleSwitch(deviceAddress) {
  console.log("Toggling switch of", deviceAddress);
  let url = "http://" + deviceAddress + "/rpc/Switch.Toggle?id=0";
  Shelly.call("HTTP.GET", {url:url});
}

Shelly.addEventHandler(function(event){
  if (event.component === "bthomedevice:200") {
    let button = event.info.idx;
    let event_name = event.info.event;
    console.log("Button", button, event_name);
    toggleSwitch(devices[button]);    
  }
});