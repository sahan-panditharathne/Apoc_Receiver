//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include <SPI.h>
#include "FS.h"
#include "SD.h"
#include "BluetoothSerial.h"
#include "RTClib.h"

//#define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
const char *pin = "1234"; // Change this to more secure PIN.

String device_name = "ESP32-BT-Slave";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;
RTC_DS1307 rtc;


void setup() {
  Serial.begin(115200);
  SerialBT.begin(device_name); //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  //Serial.printf("The device with name \"%s\" and MAC address %s is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str(), SerialBT.getMacString()); // Use this after the MAC method is implemented
  #ifdef USE_PIN
    SerialBT.setPin(pin);
    Serial.println("Using PIN");
  #endif
  
  MicroSD_init();

}

void loop() {
  if (SerialBT.available()) {
    // Read incoming command
    String command = SerialBT.readStringUntil('\n');
    Serial.print("Received command: ");
    Serial.println(command);

    // Check if the command starts with "SYNC"
    if (command.startsWith("SYNC")) {
      // Extract the date from the command
      String date = command.substring(5);
      date.trim(); // Remove leading/trailing whitespaces

      // Construct the file path based on the date
      String filePath = "/data/" + date + ".txt";
      
      // Check if the file exists
      if (SD.exists(filePath)) {
        // Read and transmit the file content over Bluetooth
        readFileOverBluetooth(SD, filePath, SerialBT);
      } else {
        Serial.println("File not found");
        SerialBT.println("ERROR: File not found");
      }
    }else if (command.startsWith("LIST")) {
      /*
      Retrives the last N number of 
      expected command syntax LIST N
      N - number of files - integer
      */
      int count = command.substring(5).toInt();
      listFiles(SD, "/data", count, SerialBT);

    }else if (command.startsWith("SETTIME")) {
      /*
      Sets the RTC time
      expected command syntax SETTIME unixtimestamp
      */
      unsigned long unixtime = command.substring(8).toInt();
      rtc.adjust(DateTime(unixtime));
      Serial.println("RTC time set");
      SerialBT.println("SUCCESS: time set!");

    } else {
        SerialBT.println("Invalid command");
    }
  }
  delay(20);
}

