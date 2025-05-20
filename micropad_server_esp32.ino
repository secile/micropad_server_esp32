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

// callback for BLE Server connect/disconnect.
class MyServerCallbacks : public BLEServerCallbacks {
  // void onConnect(BLEServer *pServer) { };

  void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t* param) {
    // set the connection interval to the minimum of 7.5ms (6 * 1.25ms)
    pServer->updateConnParams(param->connect.remote_bda, 6, 6, 0, 100);
    deviceConnected = true;
    Serial.println("onConnect");
  };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
    pServer->startAdvertising();
    Serial.println("onDisconnect. start advertising.");
  }
};

// callback to inform of events.
class MyCallbacks : public BLECharacteristicCallbacks {
public:
  MyCallbacks(void (*onReceived)(String, String, String)) {
    this->onReceived = onReceived;
  }

private:
  void (*onReceived)(String, String, String);

  // called when ESP32 receive data from micro:pad.
  void onWrite(BLECharacteristic *pCharacteristic) {
    String rxValue = pCharacteristic->getValue();

    // parse csv.
    int comma1_idx = rxValue.indexOf(',', 0);
    int comma2_idx = rxValue.indexOf(',', comma1_idx + 1);
    if (comma1_idx != -1 && comma2_idx != -1) {
      // csv with 3 params 'ControlID, Value1, Value2\n'.
      String id = rxValue.substring(0, comma1_idx);
      String value1 = rxValue.substring(comma1_idx + 1, comma2_idx);
      String value2 = rxValue.substring(comma2_idx + 1, rxValue.length()-1); // trim last \n.
      this->onReceived(id, value1, value2);
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
  pRxCharacteristic->setCallbacks(new MyCallbacks(receiveFromMicropad));

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();

  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  delay(500);
}

// send cmd to micro:pad.
void sendToMicropad(String id, String cmd) {
  if (!deviceConnected) return;
  pTxCharacteristic->setValue(id + ":" + cmd);
  pTxCharacteristic->notify();
}

// receive cmd from micro:pad.
void receiveFromMicropad(String controlId, String value1, String value2) {
  // debug. delete if you do not need.
  Serial.println("id:" + controlId + ", value1:" + value1 + ", value2:" + value2);
  
  // branch by controlID and do your task.
  if (controlId == "s1") { // slider
    // for example, pwm LED.
    // int value = value1.toFloat() * 255;
    // ledcWrite(LED_PIN, value);
  }
}