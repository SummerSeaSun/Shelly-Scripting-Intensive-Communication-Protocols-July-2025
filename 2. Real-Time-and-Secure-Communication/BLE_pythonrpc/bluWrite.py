#!/bin/python
"""
Copyright (C) 2025 : Chiara Fornoni - chiara@tecnoelettricabrianza.it

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
"""
import asyncio
import json
import struct
import logging
from bleak import BleakClient, BleakError, BleakScanner

import time
import os
import urllib.request


logger = logging.getLogger()
logger.setLevel(logging.INFO)

handler = logging.StreamHandler()  # console output
formatter = logging.Formatter("%(levelname)-8s | %(lineno)4d | %(message)s")
handler.setFormatter(formatter)
logger.addHandler(handler)


logger.debug("check rpc docs at: https://github.com/mongoose-os-libs/rpc-gatts")
TX_CTL_UUID = "5f6d4f53-5f52-5043-5f74-785f63746c5f"
DATA_UUID = "5f6d4f53-5f52-5043-5f64-6174615f5f5f"
RX_CTL_UUID = "5f6d4f53-5f52-5043-5f72-785f63746c5f"


async def scan_ble_devices(timeout=5):
    logger.info(f"BLE scan for {timeout} seconds...")
    devices = await BleakScanner.discover(timeout=timeout)
    result = [(d.address, d.name or "unknown") for d in devices]
    logger.info(f"Found {len(result)} devices")
    for mac, name in result:
        logger.info(f"{mac} -> {name}")
    return result


def scan():
    return asyncio.run(scan_ble_devices())


async def send_rpc(client, payload):
    data = json.dumps(payload).encode()
    length = len(data)
    length_be = length.to_bytes(4, "big")  

    logger.info(f"payload length: {length} byte (big-endian: {length_be.hex()})")
    await client.write_gatt_char(TX_CTL_UUID, length_be, False)
    await client.write_gatt_char(DATA_UUID, data, False)
    logger.info(f"payload sent: {payload}")


async def call_ble_rpc_async(mac, payload):
    try:
        async with BleakClient(mac) as client:
            if not client.is_connected:
                logger.error("connection failed")
                return None

            logger.info("connected")
            await send_rpc(client, payload)

            logger.debug("Read lenght reply from RX_CTL (big-endian)")
            rx_len_raw = await client.read_gatt_char(RX_CTL_UUID)
            rx_len = int.from_bytes(rx_len_raw, "big")
            logger.info(f"wait {rx_len} byte reply")

            if rx_len == 0:
                logger.warning("nessuna risposta disponibile")
                return None

            logger.debug("Read reply from DATA_UUID, may need chunked read")

            reply_raw = bytearray()
            while len(reply_raw) < rx_len:
                chunk = await client.read_gatt_char(DATA_UUID)
                reply_raw += chunk
                logger.debug(
                    f"received chunk: {len(chunk)} byte, total: {len(reply_raw)}/{rx_len}"
                )

            logger.info(f"received full ({len(reply_raw)} byte)")
            return json.loads(reply_raw)

    except BleakError as e:
        logger.error(f"errore BLE: {e}")
    except Exception as e:
        logger.error(f"errore generico: {e}")
    return None


def call_ble_rpc(mac, payload):
    return asyncio.run(call_ble_rpc_async(mac, payload))


def toggle_switch(mac):
    payload = {"id": 2, "src": "cli", "method": "Switch.Toggle", "params": {"id": 0}}
    return call_ble_rpc(mac, payload)


def set_gw_trv_temperature(mac, new_temp):
    """
    Send setpoin over gateway same as http://192.168.33.1/rpc/BluTrv.Call?id=200&method="TRV.SetTarget"&params={"id":0,"target_C":22}
    """
    call_ble_rpc(
        mac,
        {
            "id": 200,
            "src": "cli",
            "method": "BluTrv.Call",
            "params": {
                "id": 0,
                "method": "TRV.SetTarget",
                "params": {"id": 0, "target_C": new_temp},
            },
        },
    )


def set_trv_temperature(mac, new_temp):
    """
    Direct send setpoin to TRV 
    """
    return call_ble_rpc(
        mac,
            {
                "id": 1,
                "src": "postman",
                    "method": "TRV.SetTarget",
                    "params": {
                        "id": 0,
                        "target_C": new_temp
                    }
                }
    )

if __name__ == "__main__":
    logger.info("start")    
    MAC_TRV = "xx:xx:xx:xx:xx"  # -> SBTR-EU2321" / Shelly Blue TRV
    MAC_PRO = "xx:xx:xx:xx:xx"  # ShellyPro2PM-xxx
    MAC_GW = "xx:xx:xx:xx:xx"  # ShellyBluGwG3-xxx


    scan()  # show bluetooth devices near by

    # status_payload = {"id": 1, "src": "cli", "method": "Shelly.GetStatus"}
    # result_status = call_ble_rpc(MAC_PRO, status_payload)
    # if result_status:
    #     logger.info("Status of pro: %s", json.dumps(result_status, indent=2))

    # result_toggle = toggle_switch(MAC_PRO)
    # if result_toggle:
    #     logger.info("Toggle on pro: %s", json.dumps(result_toggle, indent=2))

    # result_status = call_ble_rpc(MAC_GW, status_payload)
    # if result_status:
    #     logger.info("Status of blu gateway: %s", json.dumps(result_status, indent=2))

    # res = set_gw_trv_temperature(MAC_GW, 22)
    # if res:
    #     logger.info("Set temperature via blu gateway: %s", json.dumps(res, indent=2))

    # result_status = call_ble_rpc(MAC_TRV, status_payload)
    # if result_status:
    #     logger.info("Status of TRV: %s", json.dumps(result_status, indent=2))

    res = set_trv_temperature(MAC_TRV, 23)
    if res:
        logger.info("Set temperature direct on TRV: %s", json.dumps(res, indent=2))
