
# LED-senior-design

## In Part With WPD

### Description

The purpose of this project is to establish communication between a PIR sensor  
And an LED display.  

This project is planned to be implemented in areas near roads that exhibit  
Heavy foot traffic in order to mitigate accidents.  

This project makes use of **IoT** and built-in radio communication from the **Arduino Feather m0**  
In order to serve its purpose.  

### Folder Structure

The project is split into two parts; LoRa and RF. These sections are also split into 2 parts;  
PIR and Display. The LoRa and RF split is based on the method of communication being used.  
The PIR and Display split is the different code for each board respective of their function.

### Prerequisites

- Using **Arduino Feather m0**  
- Have Arduino IDE installed  
- Have following dependencies installed/ready to use  
  - RadioHead: https://github.com/PaulStoffregen/RadioHead  
  - Arduino lmic: https://github.com/matthijskooijman/arduino-lmic  
  - Arduino SPI : https://github.com/mikaelpatel/Arduino-SPI  

### Installation/Use

Depending on what type of communication is being used, upload the respective code to each board  
using the Arduino IDE.  

- If using IOT  
  -  All Keys and EUIs will need to be changed to match the ones being used by your service.  
 
 After upload and setup is complete, project should be running and functional.  
