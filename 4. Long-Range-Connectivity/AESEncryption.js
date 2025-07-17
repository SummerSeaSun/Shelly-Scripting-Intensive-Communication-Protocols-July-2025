function uint8_to_a(buffer) {
  let result = "";
  for (let i=0; i<buffer.length;i++) {
    result += String.fromCharCode(buffer[i]);
  }
  return result;
}

const AES_KEY = "1c5wzN5g6oZ0PiwncZTGGDgYDfa8L52w";
let text = "This is a very very secret text!";
//CBC requires the length in bytes to be dividable by 16 bytes
let encryptedText = AES.encrypt(text, AES_KEY, { mode: "CBC" });
console.log("Encrypted buffer:" + encryptedText);
console.log("Encrypted text:" + uint8_to_a(encryptedText));

let decryptedText = AES.decrypt(encryptedText, AES_KEY, { mode: "CBC"});
console.log("Decrypted buffer:" + decryptedText);
console.log("Decrypted text:" + uint8_to_a(decryptedText));