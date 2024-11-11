# QC Final Counter

Counter machine for final QC which counts the results of good sewing, rework, and reject goods.

![image](https://user-images.githubusercontent.com/104487851/206646103-96eb90f2-00ed-452a-aa31-6445f2f1f605.png)

## Overview

This project is a QC final counter machine developed using an ESP8266 microcontroller, RFID reader, IR remote sensor, OLED display, and LCD display. The system is designed to connect to a server for data monitoring and control through a Wi-Fi connection. This counter helps in tracking the output of a production line, recording data such as line output, good quality items, rework items, rejected items, and balance.

## Features

- **RFID Reading**: The system reads RFID cards to identify specific QC personnel or production details.
- **IR Remote Control**: Supports IR remote input to handle various command functions.
- **Server Connectivity**: Communicates with a remote server to retrieve and display production data.
- **Display Output**: Shows data on an OLED and an I2C LCD display for clear visibility of production metrics.
- **Audio Feedback**: Provides audio feedback using an LED buzzer.
  
## Components

- **Microcontroller**: ESP8266 Wi-Fi module
- **RFID Reader**: RDM6300
- **IR Receiver**: For receiving commands from an IR remote
- **OLED Display**: Adafruit SSD1306 (128x64 pixels)
- **LCD Display**: 16x2 I2C LCD
- **Buzzer LED**: For audio feedback on button presses

## Libraries Used

- `Arduino.h`: Core library for Arduino functions
- `rdm6300.h`: For interacting with the RDM6300 RFID reader
- `ESP8266WiFi.h`: For Wi-Fi connectivity on the ESP8266
- `TextFinder.h`: For parsing text from server responses
- `IRremote.h`: For receiving and decoding IR signals
- `Wire.h`: For I2C communication with the LCD and OLED displays
- `LiquidCrystal_I2C.h`: For controlling the I2C LCD
- `Adafruit_GFX.h` and `Adafruit_SSD1306.h`: For the OLED graphics

## Hardware Configuration

- **RDM6300_RX_PIN**: 13 (for receiving RFID data)
- **READ_LED_PIN**: 16 (indicates RFID reading status)
- **LED_B**: D10 (for audio feedback)
- **IR_PIN**: D5 (for receiving IR remote commands)

## Code Explanation

### Setup

The setup function configures the display, initializes Wi-Fi, and sets up the RFID and IR components. It connects to a specified Wi-Fi network and retrieves initial production data from the server.

### Functions

- **`beep(int bep)`**: Controls the LED buzzer, which provides audio feedback for interactions.
- **`bacadata()`**: Fetches and parses data from the server to display on both the OLED and LCD.
- **`tombol(String key)`**: Sends commands to the server when an IR button is pressed, triggering specific actions based on the button.
- **`decodeIR()`**: Handles input from the IR remote, calling the appropriate function based on the button pressed.
- **`rfid()`**: Reads RFID data and sends it to the server for processing, displaying relevant data.

### Display Data

The production data is shown on two displays:
1. **OLED**: Displays the employee name, line, output, good sewing count, rework count, reject count, and balance.
2. **LCD**: Displays line and output status in real-time, along with server connection statuses.

### Sample Code Snippet

Below is a sample snippet that sends data to the server and displays production metrics:

```cpp
void bacadata(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  if (!client.connect(host, port)) {
    Serial1.println("Server Unconnect");
    lcd.setCursor(0,0);
    lcd.print("SERVER UNCONNECT");
    lcd.setCursor(0,1);
    lcd.print("  PLEASE  WAIT  ");
    beep(100);
    Reset();
  }
  urlText = "/robotik/monitoring/lihatdataQC.php?id_device=";
  urlText += id_device;
  client.print(String("GET ")+urlText+" HTTP/1.1\r\n" +"Host: "+host+ "\r\n" +"Connection: close\r\n\r\n");
  // Additional code for parsing and displaying server data here
  display.display();
}
