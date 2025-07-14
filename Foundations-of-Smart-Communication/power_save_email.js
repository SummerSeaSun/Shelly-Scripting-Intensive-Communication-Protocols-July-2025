const BREVO_API_KEY = "{API-KEY}";
const BREVO_EMAIL_API_URL = "https://api.brevo.com/v3/smtp/email";

function sendEmailNotification(to, subject, message) {
    let body = {
        sender: {
            "name": "Shelly",
            "email": "shelly.softuni@gmail.com"
        },
        to: [
            {
                "name": "Shelly",
                "email": to
            }
        ],
        "subject": subject,
        "htmlContent": message
    };
    let headers = {
        "api-key": BREVO_API_KEY
    };
    Shelly.call("HTTP.Request", { method: "POST", headers: headers, url: BREVO_EMAIL_API_URL, body: body },
        function (response) {
            console.log(response.body);
        });
}

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
                    sendEmailNotification("shelly.softuni@gmail.com", "Switch turned off",
                        "Switch was turned off after 5 minutes");
                    timerHandler = undefined;
                });
            }
        }
    }
});