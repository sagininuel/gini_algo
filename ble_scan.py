import asyncio
from bleak import BleakScanner, BleakClient

from collections import OrderedDict

async def simple_scan():
    devices = await BleakScanner.discover(return_adv=True)
    sorted_devices = OrderedDict(sorted(devices.items(), key = lambda item : item[1][1].rssi, reverse=True))

    #for device, advertisment_data in devices.values():
    for i, (address, (device, advertisement)) in enumerate(sorted_devices.items()):
        print(f"\033[92m[{i}]\033[0m Address: {address}\tDevice Name: \033[92m{device.name}\033[0m\n\tMetadata:{advertisement}")



async def generic_scan():
    devices = await BleakScanner.discover(return_adv=True)

    for i, (address, (device,metadata)) in enumerate(devices.items()):
        if device.name == "DW9489":

            print()
            print(f"Name: {device.name}")
            print(f"Address: {device.address}")
            print(f"Service Data: {metadata.service_data}")
            print(f"Service UUIDs: {metadata.service_uuids}")
            print(f"RSSI: {metadata.rssi}")


    for address, (device, metadata) in devices.items():
        try:
            this_device = await BleakScanner.find_device_by_address(device.address, timeout=5)
            
            
            ble_address = "E6:27:2F:B5:A7:EC"
            async with BleakClient(this_device) as client:
                print(f"Services found for device")
                print(f"\tDevice address: \033[93m{device.address}\033[0m")
                print(f"\tDevice name: \033[92m{device.name}\033[0m")

                print("\tServices:")

                for service in client.services:
                    print()
                    print(f"\t\tDescription: {service.description}")
                    print(f"\t\tService: {service}")

                    print("\t\tCharacteristics:")

                    for characteristic in service.characteristics:
                        print(f"\t\t\tUUID: {characteristic.uuid}")

                        print('\t\tDescriptors:')
                        for decriptor in characteristic.descriptors:
                            print(f"\t\t\t{descriptor}")
        except Exception as e:
                print(f"\033[91mCould not connect to device with info: {device}\033[0m")
                print(f"\033[91mError: {e}\033[0m")


async def scan_known():
    ble_address = "E6:27:2F:B5:A7:EC"
    
    async with BleakClient(ble_address) as client:
        print("Connected ..")
        print(client.is_connected)

        print("\tServices:")

        for service in client.services:
            print()
            print(f"\tService UUID: \033[92m{service}\033[0m")
            print(f"\tService Description: \033[0m{service.description}\033[0m")


            for characteristic in service.characteristics:
                print("\t\tCharacteristic:")
                print(f"\t\tUUID: \033[93m{characteristic.uuid}\033[0m")
                for descriptor in characteristic.descriptors:
                    print('\t\t\tDescriptor:')
                    print(f"\t\t\t\033[92m{descriptor}\033[0m")


def read_dev_info(payload):
    (node, hw_version, fw1_version, fw2_version, fw1_checksum, fw2_checksum, flags) = (
    hex((int).from_bytes(payload[0:7], 'little')),
    hex((int).from_bytes(payload[8:11], 'little')),
    hex((int).from_bytes(payload[12:15], 'little')),
    hex((int).from_bytes(payload[16:19], 'little')),
    hex((int).from_bytes(payload[20:23], 'little')),
    hex((int).from_bytes(payload[24:27], 'little')),
    hex((int).from_bytes(payload[28:29], 'little')),
    )

    print(f"Node ID: {node}\n \
            HW version: {hw_version}\n \
            FW1 version: {fw1_version}\n \
            FW2 version: {fw2_version}\n \
            FW1 checksum: {fw1_checksum}\n \
            FW2 checksum: {fw2_checksum}\n \
            RDonly Operation flags: {flags}\n")

def read_raw_data(payload):
    print(payload)


ble_address = "E6:27:2F:B5:A7:EC"

#Characteristic uuids
device_info = "1E63B1EB-D4ED-444E-AF54-C1E965192501"
location_data_mode = "a02b947e-df97-4516-996a-1882521e0ead"
location_data = "003bbdf2-c634-4b3d-ab56-7ec889b89a37"
    
async def read():

    async with BleakClient(ble_address) as client:
        data = await client.read_gatt_char(location_data)
        #dev_info(data)
        read_raw_data(data)

async def write():

    async with BleakClient(ble_address) as client:
        before_data = await client.read_gatt_char(location_data_mode)
        read_raw_data(before_data)
        
        update = before_data
        update[0] = 1
        await client.write_gatt_char(location_data_mode, update)
        after_data = await client.read_gatt_char(location_data_mode)
        read_raw_data(after_data)



if __name__=="__main__":
    asyncio.run(simple_scan())




