let str = "Hello" + String.fromCharCode(0) + " World!" + String.fromCharCode(247) ;
let b64encoded = btoa(str);
console.log("Encoded:", b64encoded);
console.log(str.length);
console.log(b64encoded.length);
let decoded = atob(b64encoded);
console.log("Decoded:", decoded);

let obj = {
  // cannot use str directly - need to encode it first
  //data: str
  data: b64encoded
}

console.log(JSON.stringify(obj));