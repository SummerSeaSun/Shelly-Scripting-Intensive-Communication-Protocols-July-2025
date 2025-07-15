const http = require('http')
const ShellyOWS = require("../shellyOWS");

console.log('WS server starting at port 8080');

const httpServer = http.createServer();
let shellyOws = new ShellyOWS(httpServer);

async function logNotifyEvent(clientId, params) {
    console.log("NotifyEvent");
    console.log("Client ID:", clientId);
    console.log("Params:", params);

    if (params.events[0].component==="button:200" && params.events[0].event==="single_push") {
        console.log("virtual button pushed");
        await shellyOws.call(clientId, "Switch.Toggle", {id:0});
    }
}

async function logNotifyStatus(clientId, params) {
    console.log("NotifyStatus");
    console.log("Client ID:", clientId);
    console.log("Params:", params);
}
async function logNotifyFullStatus(clientId, params) {
    console.log("NotifyFullStatus");
    console.log("Client ID:", clientId);
    console.log("Params:", params);
}

shellyOws.addHandler("NotifyStatus", logNotifyStatus);
shellyOws.addHandler("NotifyFullStatus", logNotifyFullStatus);
shellyOws.addHandler("NotifyEvent", logNotifyEvent);

httpServer.listen(8080);
