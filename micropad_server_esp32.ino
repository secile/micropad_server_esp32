/*
  based on BLE UART example.
  
  Copyright (c) 2025 secile
  This software is released under the MIT license.
  see https://opensource.org/licenses/MIT
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
  }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      Serial.println("*********");
      Serial.print("Received Value: ");
      for (int i = 0; i < rxValue.length(); i++) {
        Serial.print(rxValue[i]);
      }

      Serial.println();
      Serial.println("*********");
    }
  }
};

void setup() {
  Serial.begin(115200);

  initBLE();
}

void initBLE() {
  // +----------------------------+           +----------------------------+
  // | [micro:bit, ESP32]         |           | [micropad on Smartphone]   |
  // | as Peripheral.             |           | as Central.                |
  // | do Advertise.              |           | do Scan.                   |
  // | become Server.             |           | become Client.             |
  // | expose Nordic UART Service.|           | connect to Service.        |
  // | - TX (0002) [Nofity, CCCD] | --------> | - TX (0002) start notify.  |
  // | - RX (0003) [Write]        | <-------- | - RX (0003) write value.   |
  // +----------------------------+           +----------------------------+

  // reverse TX and RX to match the micro:bit.
  const char* UART_SERVICE_UUID      = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
  const char* CHARACTERISTIC_UUID_TX = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";
  const char* CHARACTERISTIC_UUID_RX = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";

  // micro:pad only supports device name starts with "BBC micro:bit".
  // if you change device name, you also have to change micro:pad source code below.
  // await navigator.bluetooth.requestDevice({ filters: [{ namePrefix: "BBC micro:bit" }], ...
  const char* BLE_DEVICE_NAME = "BBC micro:bit ESP32";

  // Create the BLE Device
  BLEDevice::init(BLE_DEVICE_NAME);

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(UART_SERVICE_UUID);

  // TX (0002) ESP32(server) -> micropad(client)
  pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
  pTxCharacteristic->addDescriptor(new BLE2902());
  
  // RX (0003) ESP32(server) <- micropad(client)
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();

  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  delay(500);
}
