const AES_KEY = '60e1b337fc8500d5e1eb65a0ea66e762ab893e46efe09a9d162e65171929b45d';
const CHECKSUM_SIZE = 4;

const MY_ID = Shelly.getDeviceInfo().mac;

function runRPC(encodedRPC) {
  let splitRpc = encodedRPC.split("|");
  if (splitRpc.length !== 3) {
    console.log("Non RPC message. Skipping.");
    return;
  }
  let dst = splitRpc[0];
  
  if (dst!==MY_ID) {
    console.log("this packet is not for me");
    return;
  }
  let method = splitRpc[1];
  eval("params = " + splitRpc[2]);
  Shelly.call(method, params);
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

function verifyMessage(message) {
  if (message.length < CHECKSUM_SIZE + 1) {
    console.log('[LoRa] invalid message (too short)');
    return;
  }

  const receivedCheckSum = message.slice(0, CHECKSUM_SIZE);
  const _message = message.slice(CHECKSUM_SIZE);
  const expectedChecksum = generateChecksum(_message);

  //console.log("_Message:", _message);
  //console.log("Received checksum:", receivedCheckSum);
  //console.log("expected checksum:", expectedChecksum)

  if (receivedCheckSum !== expectedChecksum) {
    console.log('[LoRa] invalid message (checksum corrupted)');
    return;
  }

  return _message;
}

function fromHex(hex) {
  const arr = new ArrayBuffer(hex.length / 2);
  for (let i = 0; i < hex.length; i += 2) {
    arr[i / 2] = parseInt(hex.substr(i, 2), 16);
  }
  return arr;
}

function uint8_to_a(buffer) {
  let result = "";
  let msg = "";
  for (let i=0; i<buffer.length;i++) {
    if (i>=4) msg+String.fromCharCode(buffer[i]);
    result += String.fromCharCode(buffer[i]);
  }
  //console.log("Message:",msg);
  return result;
}

function decryptMessage(buffer, keyHex) {
  const key = fromHex(AES_KEY);
  const decrypted = AES.decrypt(buffer, key, { mode: 'CFB' });
  //console.log(decrypted);

  if (!decrypted || decrypted.byteLength === 0) {
    console.log('[LoRa] invalid msg (empty decryption result)');
    return;
  }

  const checksumMessage = uint8_to_a(decrypted);
  console.log("ChecksumMsg:", checksumMessage);
  const finalMessage = verifyMessage(checksumMessage);
  
  return finalMessage;
}

//test
//runRPC(MY_ID|Switch.Toggle|{id:0}');
Shelly.addEventHandler(function(event){
  if (event.component==="lora:100" && event.info.event==="lora_received") {
    let data = event.info.data;
    //decode b64 message
    let msg = atob(data);
    let decryptedMessage = decryptMessage(msg);
    if (!decryptedMessage) {
      console.log("Error decrypting message");
      return;
    }
    console.log("Message received:" + decryptedMessage);
    runRPC(decryptedMessage);
  }
});