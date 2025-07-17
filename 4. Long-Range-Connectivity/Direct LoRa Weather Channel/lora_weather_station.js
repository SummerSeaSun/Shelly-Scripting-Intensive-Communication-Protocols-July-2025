const SENSORS_METADATA = [
    { name: "temperature", bytes_count: 2, factor: 0.01, unit: "C" },
    { name: "humidity", bytes_count: 2, factor: 0.01, unit: "%" },
    { name: "pressure", bytes_count: 3, factor: 0.01, unit: "hPa" },
    { name: "wind_speed", bytes_count: 2, factor: 0.01, unit: "m/s" },
    { name: "wind_gust", bytes_count: 2, factor: 0.01, unit: "m/s" },
    { name: "precipitation", bytes_count: 2, factor: 0.1, unit: "mm/h" },
];

const ENCRYPTION_ENABLED = false;
const aesKey = 'dd469421e5f4089a1418ea24ba37c61bdd469421e5f4089a1418ea24ba37c61b';

function fromHex(hex) {
    const arr = new ArrayBuffer(hex.length / 2);
    for (let i = 0; i < hex.length; i += 2) {
        arr[i / 2] = parseInt(hex.substr(i, 2), 16);
    }
    return arr;
}

function toHex(buffer) {
    let s = '';
    for (let i = 0; i < buffer.length; i++) {
        s += (256 + buffer[i]).toString(16).substr(-2);
    }
    return s;
}

function aToBuffer(str) {
    let buffer = new ArrayBuffer(str.length);
    for (let i = 0; i < str.length; i++) {
        buffer[i] = str.charCodeAt(i);
    }
    return buffer;
}

function decryptMessage(buffer, keyHex) {
    const key = fromHex(keyHex);
    const iv = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    const decrypted = AES.decrypt(buffer, key, { mode: 'CFB', iv: iv });

    if (!decrypted || decrypted.byteLength === 0) {
        console.log('[LoRa] invalid msg (empty decryption result)');
        return;
    }

    return decrypted;
}

function decodeWeatherData(buffer) {
    buffer = aToBuffer(buffer);
    console.log("Base64 decoded input:", toHex(buffer));
    if (ENCRYPTION_ENABLED) {
        buffer = decryptMessage(buffer, aesKey);
        console.log("Decrypted:", toHex(buffer));
    }
    let buf_idx = 0;
    let result = {};
    for (sensor of SENSORS_METADATA) {
        let value = 0;
        let bytes_count = sensor.bytes_count;
        for (let j = 0; j < bytes_count; j++) {
            value = (value << 8) | buffer[buf_idx + bytes_count - 1 - j];
        }
        buf_idx += bytes_count;

        value *= sensor.factor;
        result[sensor.name] = {
            //          name: sensor.name,
            unit: sensor.unit,
            value: value
        };
    }
    console.log(JSON.stringify(result));
}

Shelly.addEventHandler(function (event) {
    if (event.component === "lora:100") {
        console.log("Raw data:", event.info.data);
        decodeWeatherData(atob(event.info.data));
    }
});