function decodeHelper(buffer) {
    let result = '';
    for (let i = 0; i < buffer.length; i++) {
        result += String.fromCharCode(buffer[i]);
    }
    return result;
}

const key = "ClWd5V1ihP8mXm9clM9GDuopOPXxrIx0";
let cypherText = AES.encrypt("This is one big, big secret no one should know!!", key, { mode: "CBC" });

let text = AES.decrypt(cypherText, key, { mode: "CBC" });
console.log(decodeHelper(text));