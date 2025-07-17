// refer to https://shelly-api-docs.shelly.cloud/gen2/Addons/ShellyLoRaAddon#configuration
// for more configuration options
let config = {
  freq: 866000000,
  dr: 12
}

Shelly.call("Lora.SetConfig", {id:100, config: config}, function(result){
  console.log(result);
});