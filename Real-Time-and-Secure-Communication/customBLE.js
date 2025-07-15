const CONFIG = {
  // Specify the destination event where the decoded BLE data will be emitted. It allows for easy identification by other applications/scripts
  eventName: "shelly-blu",

  // If this value is set to true, the scan will be active.
  // If this value is set to false, the scan will be passive.
  // Active scan means the scanner will ping back the Bluetooth device to receive all its data, but it will drain the battery faster
  active: true,

  // When set to true, debug messages will be logged to the console
  debug: true,
  temperature: 200,
  humidity: 201,
  pressure: 202,
  wind_speed: 203,
  wind_gust: 204,
  precipitation: 205
};

const SVC_ID_STR = "a3e87a59-fbd2-40a5-9450-a6b1a8d67059";
const SENSORS_METADATA = [
    {name: "temperature", bytes_count: 2, factor: 0.01, unit: "C"},
    {name: "humidity", bytes_count: 2, factor: 0.01, unit: "%"},
    {name: "pressure", bytes_count: 3, factor: 0.01, unit: "hPa"},
    {name: "wind_speed", bytes_count: 2, factor: 0.01, unit: "m/s"},
    {name: "wind_gust", bytes_count: 2, factor: 0.01, unit: "m/s"},
    {name: "precipitation", bytes_count: 2, factor: 0.1, unit: "mm/h"},

];

function updateVC(updatedObj, id) {
  if (updatedObj) {
    let updateValue = updatedObj.value;
    let currentValue = Shelly.getComponentStatus("number:" + id).value;
    console.log("Current Value:", currentValue, "Updated Object:", JSON.stringify(updatedObj));
    
    if (updateValue !== currentValue ) {
      Shelly.call("Number.Set", {id: id, value: updateValue }, function(result) {
        console.log(result);
      });
    }
  }
}

function readSensorData(data) {
  let updatedTemp = data.temperature;
  let updatedHumidity = data.humidity;
  let updatedPressure = data.pressure;
  let updatedWindSpeed = data.wind_speed;
  let updatedWindGust = data.wind_gust;
  let updatedPrecipitation = data.precipitation;
  
  updateVC(updatedTemp, CONFIG.temperature);
  updateVC(updatedHumidity, CONFIG.humidity);
  updateVC(updatedPressure, CONFIG.pressure);
  updateVC(updatedWindSpeed, CONFIG.wind_speed);
  updateVC(updatedWindGust, CONFIG.wind_gust);
  updateVC(updatedPrecipitation, CONFIG.precipitation);
  
}

// functions for decoding and unpacking the service data from Shelly BLU devices
const BLEDecoder = {
  // Unpacks the service data buffer from the device
  unpack: function (buffer) {
    if (typeof buffer !== "string" || buffer.length === 0) return null;

    //let pid = (buffer.at(1) << 8) | buffer.at(0);

    let result = {
//      pid: pid
    };

    let buf_idx = 0;
    for (sensor of SENSORS_METADATA) {
      let value = 0;
      let bytes_count = sensor.bytes_count;
      for (let j=0; j<bytes_count; j++) {
        value = (value << 8) | buffer.at(buf_idx + bytes_count - 1 - j);
      }
      buf_idx+=bytes_count;
      
      value *= sensor.factor;
      result[sensor.name]= {
//          name: sensor.name,
          unit: sensor.unit,
          value: value
        };
    }
    return result;
  },
};

//saving the id of the last packet, this is used to filter the duplicated packets
let lastPacketId = 0xffff;

// Callback for the BLE scanner object
function BLEScanCallback(event, result) {
  //exit if not a result of a scan
  if (event !== BLE.Scanner.SCAN_RESULT) {
    return;
  }
  
  //if (result.addr=="a0:dd:6c:74:7f:02")  {
    //let r = result.service_data["2a1f"];
//    console.log(result);
    //console.log("data:", r, r.length);
  //}

  //exit if service_data member is missing
  if (
    typeof result.service_data === "undefined" ||
    typeof result.service_data[SVC_ID_STR] === "undefined"
  ) {
    return;
  }
  
  let unpackedData = BLEDecoder.unpack(
    result.service_data[SVC_ID_STR]
  );

  //exit if unpacked data is null or the device is encrypted
  if (
    unpackedData === null ||
    typeof unpackedData === "undefined" ||
    unpackedData["encryption"]
  ) {
    console.log("Error: Encrypted devices are not supported");
    return;
  }

  //exit if the event is duplicated
  //if (lastPacketId === unpackedData.pid) {
  //  return;
  //}
  //lastPacketId = unpackedData.pid;

  unpackedData.rssi = result.rssi;
  unpackedData.address = result.addr;
  unpackedData.model = result.local_name;
  
  // update virtual components value
  console.log(JSON.stringify(unpackedData));
  readSensorData(unpackedData);
}

// Initializes the script and performs the necessary checks and configurations
function init() {
  //exit if can't find the config
  if (typeof CONFIG === "undefined") {
    console.log("Error: Undefined config");
    return;
  }

  //get the config of ble component
  const BLEConfig = Shelly.getComponentConfig("ble");

  //exit if the BLE isn't enabled
  if (!BLEConfig.enable) {
    console.log(
      "Error: The Bluetooth is not enabled, please enable it from settings"
    );
    return;
  }

  //start the scanner
  const bleScanner = BLE.Scanner.Start({
    duration_ms: BLE.Scanner.INFINITE_SCAN,
    active: CONFIG.active
  });

  if (!bleScanner) {
    console.log("Error: Can not start new scanner");
  }

  //subscribe a callback to BLE scanner
  BLE.Scanner.Subscribe(BLEScanCallback);

  // disable console.log when logs are disabled
  if (!CONFIG.debug) {
    console.log = function () { };
  }
}

init();