const DOOR_SENSOR = "bthomesensor:206"
const MOTION_SENSOR = "bthomesensor:210"
Shelly.addStatusHandler(function(event){
  if (event.component === DOOR_SENSOR) {
    let isDoorOpen = event.delta.value;
    let isMotion = Shelly.getComponentStatus(MOTION_SENSOR).value;
    if (isDoorOpen) {
      if (isMotion) {
        console.log("Goodbye/Home Exit");
      } else {
        console.log("Welcome Home");
      }
    }
  }
});