HTTPServer.registerEndpoint("info", function (request, response) {
    console.log("Received a request with the following properties:");
    console.log("Method: ", request.method);
    console.log("Query String: ", request.query);
    //  console.log("Headers: ", JSON.stringify(request.headers));
    //  console.log("Body: ", JSON.stringify(request.body));

    //  let body = JSON.parse(request.body);
    //  console.log("Action:", body.action);

    // Example of sending a response
    response.code = 200;
    response.body = JSON.stringify({ "endpoint": "Hello from info endpoint" });
    //response.headers = [["Content-Type", "text/plain"]];
    response.headers = [["Content-Type", "application/json"]];
    response.send();
});