const TEMP_COMPONENT = "bthomesensor:203";

let temperature = Shelly.getComponentStatus(TEMP_COMPONENT).value;

console.log("Current temperature is", temperature, "C");