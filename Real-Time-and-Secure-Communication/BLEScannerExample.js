BLE.Scanner.Start({"duration_ms": BLE.Scanner.INFINITE_SCAN, "active": true}, function(event, result){
  if (event === BLE.Scanner.SCAN_RESULT) {
    if (result.addr=="7c:c6:b6:61:f8:be") {
//      delete result.advData;
//      delete result.service_data;
//      console.log(JSON.stringify(result));
      console.log(result);
    }
  }
});