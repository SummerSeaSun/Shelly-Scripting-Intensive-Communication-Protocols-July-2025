# Bluetooth rpc from a raspberry 4 to shelly devices

Except Shelly BLU Gateway other shelly can manage only reading over bluetooth.

This python example should show how to manage writing over bluetooth rpc from a raspberrypi 4.

What works:
- scan() list neighbors devices
- Shelly.GetStatus
- Switch.Toggle
TODO: manage temperature setpoint on BLU TRV

___
How to run: 

```bash
pipenv install
# replace device MAC_***
pipenv run ipython bluWrite.py
```

To get more informations change logger.setLevel(logging.INFO) to logger.setLevel(logging.DEBUG)

For installation please look at https://pipenv.pypa.io/en/latest/
___

What to expect to see:
```bash
INFO     |  153 | start
INFO     |   92 | connected
INFO     |   79 | payload length: 53 byte (big-endian: 00000035)
INFO     |   82 | payload sent: {'id': 1, 'src': 'cli', 'method': 'Shelly.GetStatus'}
INFO     |   98 | wait 1474 byte reply
INFO     |  114 | received full (1474 byte)
INFO     |  163 | Status of pro: {
  "id": 1,
  "src": "shellypro2pm",
  "dst": "cli",
  "result": {
    "ble": {},
    "bthome": {},
    "cloud": {
      "connected": true
    },
    "eth": {
      "ip": null,
      "ip6": null
    },
    "input:0": {
      "id": 0,
      "state": false
    },
    "input:1": {
      "id": 1,
      "state": false
    },
    "knx": {},
    "mqtt": {
      "connected": true
    },
    "switch:0": {
      "id": 0,
      "source": "GATTS",
      "output": false,
      "apower": 0.0,
      "voltage": 1.5,
      "freq": 50.0,
      "current": 0.0,
      "pf": 0.0,
      "aenergy": {
        "total": 0.0,
        "by_minute": [
          0.0,
          0.0,
          0.0
        ],
        "minute_ts": 1753170240
      },
      "ret_aenergy": {
        "total": 0.0,
        "by_minute": [
          0.0,
          0.0,
          0.0
        ],
        "minute_ts": 1753170240
      },
      "temperature": {
        "tC": 40.3,
        "tF": 104.6
      }
    },
    "switch:1": {
      "id": 1,
      "source": "init",
      "output": false,
      "apower": 0.0,
      "voltage": 1.6,
      "freq": 50.0,
      "current": 0.0,
      "pf": 0.0,
      "aenergy": {
        "total": 0.0,
        "by_minute": [
          0.0,
          0.0,
          0.0
        ],
        "minute_ts": 1753170240
      },
      "ret_aenergy": {
        "total": 0.0,
        "by_minute": [
          0.0,
          0.0,
          0.0
        ],
        "minute_ts": 1753170240
      },
      "temperature": {
        "tC": 41.3,
        "tF": 106.3
      }
    },
    "sys": {
      "mac": "",
      "restart_required": false,
      "time": "09:44",
      "unixtime": 1753170297,
      "last_sync_ts": 1753169157,
      "uptime": 79786,
      "ram_size": 261124,
      "ram_free": 78920,
      "ram_min_free": 58976,
      "fs_size": 524288,
      "fs_free": 192512,
      "cfg_rev": 42,
      "kvs_rev": 0,
      "schedule_rev": 2,
      "webhook_rev": 2,
      "btrelay_rev": 0,
      "bthc_rev": 0,
      "available_updates": {
        "stable": {
          "version": "1.6.2"
        }
      },
      "reset_reason": 3,
      "utc_offset": 7200
    },
    "wifi": {
        ...
    },
    "ws": {
      "connected": false
    }
  }
}
INFO     |   92 | connected
INFO     |   79 | payload length: 71 byte (big-endian: 00000047)
INFO     |   82 | payload sent: {'id': 2, 'src': 'cli', 'method': 'Switch.Toggle', 'params': {'id': 0}}
INFO     |   98 | wait 80 byte reply
INFO     |  114 | received full (80 byte)
INFO     |  167 | Toggle on pro: {
  "id": 2,
  "src": "shellypro2pm",
  "dst": "cli",
  "result": {
    "was_on": false
  }
}
INFO     |   92 | connected
INFO     |   79 | payload length: 53 byte (big-endian: 00000035)
INFO     |   82 | payload sent: {'id': 1, 'src': 'cli', 'method': 'Shelly.GetStatus'}
INFO     |   98 | wait 817 byte reply
INFO     |  114 | received full (817 byte)
INFO     |  171 | Status of blu gateway: {
  "id": 1,
  "src": "shellyblugwg3",
  "dst": "cli",
  "result": {
    "ble": {},
    "blugw": {},
    "bthome": {},
    "cloud": {
      "connected": true
    },
    "mqtt": {
      "connected": false
    },
    "script:1": {
      "id": 1,
      "running": false,
      "mem_free": 25200
    },
    "script:2": {
      "id": 2,
      "running": false,
      "mem_free": 25200
    },
    "script:3": {
      "id": 3,
      "running": false,
      "mem_free": 25200
    },
    "sys": {
      "mac": "",
      "restart_required": false,
      "time": "09:45",
      "unixtime": 1753170306,
      "last_sync_ts": 1753169061,
      "uptime": 1249,
      "ram_size": 256316,
      "ram_free": 79892,
      "ram_min_free": 60068,
      "fs_size": 1048576,
      "fs_free": 573440,
      "cfg_rev": 144,
      "kvs_rev": 0,
      "schedule_rev": 1,
      "webhook_rev": 0,
      "btrelay_rev": 0,
      "available_updates": {
        "beta": {
          "version": "1.7.0-beta3"
        },
        "stable": {
          "version": "1.6.2"
        }
      },
      "reset_reason": 1
    },
    "wifi": {
        ...
    },
    "ws": {
      "connected": false
    }
  }
}
INFO     |   92 | connected
INFO     |   79 | payload length: 135 byte (big-endian: 00000087)
INFO     |   82 | payload sent: {'id': 200, 'src': 'cli', 'method': 'BluTrv.Call', 'params': {'id': 0, 'method': 'TRV.SetTarget', 'params': {'id': 0, 'target_C': 22}}}
INFO     |   98 | wait 125 byte reply
INFO     |  114 | received full (125 byte)
ERROR    |  118 | errore BLE: [org.bluez.Error.Failed] Software caused connection abort
ERROR    |  118 | errore BLE: [org.bluez.Error.Failed] Software caused connection abort
``` bash
