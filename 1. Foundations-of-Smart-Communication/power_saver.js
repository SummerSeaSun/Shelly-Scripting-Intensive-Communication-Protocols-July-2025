let timerHandler;

Shelly.addStatusHandler(function (event) {
    // check if the status change came from switch:0 and it changed the output
    if (event.component === "switch:0" && event.delta.output) {
        if (event.delta.output === true) {
            // make sure we do not start multiple timers at the same time
            if (timerHandler === undefined) {
                console.log("Switch turned on, starting off timer");
                timerHandler = Timer.set(5000, false, function () {
                    console.log("Timer expiered, turning off the switch");
                    Shelly.call("Switch.Set", { id: 0, on: false });
                    timerHandler = undefined;
                });
            }
        }
    }
});