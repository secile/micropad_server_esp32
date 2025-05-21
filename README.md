This is a ESP32 Arduino project that supports [micropad](https://github.com/secile/micropad).  
You can remotely control your ESP32 from smartphone via BLE (Bluetooth Low Energy).

# How to use.
clone this repository, or download zip, open by Arduino IDE, and upload to your ESP32.  
Access [micropad](https://secile.github.io/micropad/) page, and press '🚩' button to connect your ESP32.  
You can see your ESP32 device in pairing dialog as name 'BBC micro:bit ESP32'.

# Receive Massage from micropad.
When you operate micropad controls (Button, Analog Stick, and Slider), micropad send message to your ESP32, and 'receiveFromMicropad' method was called.
You have to modify 'receiveFromMicropad' method to handle your task.
Control ID is ID that every control have it's own. (e.g. Slider is 's1', Analog Stick is 'a1' or 'a2').
Branch by control ID and do your task.

```c
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
```

# Send Massage to micropad.
Call sendToMicropad method to send message to micropad. Normally, sent message was displayed by Label control.
For example, send message "hello" to Label1 (control id is 'l1') is following.

```c
sendToMicropad("l1", "hello");
```

