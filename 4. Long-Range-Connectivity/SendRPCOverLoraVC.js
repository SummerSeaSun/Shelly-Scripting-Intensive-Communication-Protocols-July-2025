const AES_KEY = '60e1b337fc8500d5e1eb65a0ea66e762ab893e46efe09a9d162e65171929b45d';
const CHECKSUM_SIZE = 4;

const dst = "A085E3BBEBA0";

function fromHex(hex) {
  const arr = new ArrayBuffer(hex.length / 2);
  for (let i = 0; i < hex.length; i += 2) {
    arr[i / 2] = parseInt(hex.substr(i, 2), 16);
  }
  return arr;
}

function generateChecksum(msg) {
  let checksum = 0;
  for (let i = 0; i < msg.length; i++) {
    checksum ^= msg.charCodeAt(i);
  }
  let hexChecksum = checksum.toString(16);

  while (hexChecksum.length < CHECKSUM_SIZE) {
    hexChecksum = '0' + hexChecksum;
  }
  return hexChecksum.slice(-CHECKSUM_SIZE);
}

function encryptMessage(msg, keyHex) {
  const key = fromHex(keyHex);
  const encMsg = AES.encrypt(msg, key, { mode: 'CFB' });
  return encMsg;
}

function encodeRPC(rpc) {
  // receives RPC like {"dst": "112233445566", "method":"Switch.Toggle", "params":{"id":0}}
  // the result should be: 112233445566|Switch.Toggle|{id:0}
  let encoded = rpc.dst;
  encoded += "|" + rpc.method;
  encoded += "|" + JSON.stringify(rpc.params);

  return encoded;
}

function sendLoRa(rpcCommand) {
  let encodedRPC = encodeRPC(rpcCommand);
  let message = generateChecksum(encodedRPC) + encodedRPC;
  let encryptedMessage = encryptMessage(message, AES_KEY);
  Shelly.call("LORA.SendBytes", { id: 100, data: btoa(encryptedMessage) });
}

const VC_BUTTON = "button:200";
const vcButtonHandle = Virtual.getHandle(VC_BUTTON);

let rpcButton = { "dst": dst, "method": "Switch.Toggle", "params": { "id": 0 } };
vcButtonHandle.on("single_push", function (event) {
  sendLoRa(rpcButton);
});

// temperature example. if temperature from H&T BLU sensor goes above 30C, turn the light red through LoRa
let rpcRGB = { "dst": dst, "method": "HTTP.GET", "params": { "url": "http://192.168.10.161/rpc/rgb.set?id=0&rgb=[255,0,0]&on=true" } };
const TEMP_COMPONENT = "bthomesensor:202";
const TEMP_THRESHOLD = 30;
Shelly.addStatusHandler(function (event) {
  if (event.component === TEMP_COMPONENT) {
    let temp = event.delta.value;
    console.log("Temperature reading:", temp);
    if (temp > TEMP_THRESHOLD) {
      console.log("Temperature threshold reached, changing LED to red!");
      sendLoRa(rpcRGB);
    }
  }
});