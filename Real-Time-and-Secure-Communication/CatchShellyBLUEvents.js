Shelly.addEventHandler(function(event){
  if (event.info.event === "shelly-blu") {
    console.log(JSON.stringify(event));
  }
});