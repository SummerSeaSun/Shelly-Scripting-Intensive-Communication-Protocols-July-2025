const devices = ["192.168.10.127", "192.168.10.162", "192.168.10.167", "192.168.10.190"];

function toggleSwitch(deviceAddress) {
  console.log("Toggling switch of", deviceAddress);
  let url = "http://" + deviceAddress + "/rpc/Switch.Toggle?id=0";
  Shelly.call("HTTP.GET", {url:url});
}

Shelly.addEventHandler(function(event){
  if (event.info.event === "shelly-blu" && event.info.data.address === "7c:c6:b6:73:bb:e2") {
    let buttons = event.info.data.button;
    for (let i=0; i<devices.length; i++) {
      if (buttons[i] === 1) {
        toggleSwitch(devices[i]);
      }
    }
  }
});