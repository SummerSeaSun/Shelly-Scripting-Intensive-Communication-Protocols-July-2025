Timer.set(10000, true, function () {
    let time = Shelly.getComponentStatus("sys").unixtime + (60 * 1000);
    let message = "time" + time;
    console.log("sending:", message);
    Shelly.call("lora.sendbytes", { id: 100, data: btoa(message) });
});