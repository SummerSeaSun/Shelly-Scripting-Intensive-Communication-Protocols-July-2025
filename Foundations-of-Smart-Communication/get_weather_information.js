let url = "https://api.open-meteo.com/v1/forecast?latitude=42.15&longitude=24.75&current=temperature_2m,precipitation,wind_speed_10m";
Shelly.call("HTTP.GET", { url: url }, function (response, error_code, error_message) {
    console.log(error_code, error_message);
    if (error_code != 0) {
        console.log("there was an error", error_code, error_message);
        return;
    }
    let body = JSON.parse(response.body);
    console.log("Current temperature:", body.current.temperature_2m, "C");
    console.log("Wind speed:", body.current.wind_speed_10m, "m/s");
});