Shelly.call("Switch.Set", { "id": 0, "on": false }, function (result) {
    console.log(JSON.stringify(result));
});