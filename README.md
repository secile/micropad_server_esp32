This is a ESP32 Arduino project that can be controlled by [micropad](https://github.com/secile/micropad).  
You can remotely control your ESP32 from smartphone via BLE (Bluetooth Low Energy).

# What is micropad.
[micropad](https://github.com/secile/micropad) is an virtual controller app with AnalogStick, Slider, Button, etc.
The app is built with HTML and JavaScript that can be run in the web browser of any smartphone that support the Web Bluetooth API.

![image](https://github.com/user-attachments/assets/02a81074-fa8e-4177-a1b6-99e8fae305b1)

# How to use.
- clone this repository, or download zip, open by Arduino IDE
- overwrite 'receiveFromMicropad' function to execute the necessary task.
- call 'sendToMicropad' function when you want to send message to micropad.
- upload program to your ESP32.
- access [micropad](https://secile.github.io/micropad/) page, and press '🚩' button to connect your ESP32.
  - You can see your ESP32 device in pairing dialog as name 'BBC micro:bit ESP32'.
  - ![image](https://github.com/user-attachments/assets/ad37c51d-d858-47a1-b22e-c7ea7f2363e7)

# Receive Message from micropad.
When you operate micropad controls (Button, Analog Stick, and Slider), micropad send message to your ESP32, and 'receiveFromMicropad' function was called.
You have to modify 'receiveFromMicropad' function to handle your task.  
- 1st argument is control ID.
  - control ID is ID that every control have it's own. (e.g. Slider is 's1', Analog Stick is 'a1' or 'a2').
- 2nd argument value1 and 3rd argument value2 is depend on control ID.
  - when control ID is 'a1' or 'a2' means 'Analog Stick', value1 is radian angle clockwise from the X axis, and value2 is power.
  - when control ID is 'b1' or 'b2' means 'Button', value1 is 1(on) or 0(off). value2 always 0.
  - when control ID is 's1' means 'Slider', value1 is ratio with 0.0-1.0. (0.1 step). value2 always 0.
- Branch by control ID and do your task.

```c
// called when receive message from micropad.
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
Call sendToMicropad function to send message to micropad.

```c
void loop() {
  static int loopCount = 0;
  sendToMicropad("l1", String(loopCount++));
  delay(1000);
}
```

- 1st argument is destination control ID. normally, message is sent to label and displayed on the screen.
- 2nd argument is message text that you want to send.
