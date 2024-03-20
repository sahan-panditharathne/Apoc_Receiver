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

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.print (file.name());
            time_t t= file.getLastWrite();
            struct tm * tmstruct = localtime(&t);
            Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.print(file.size());
            time_t t= file.getLastWrite();
            struct tm * tmstruct = localtime(&t);
            Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
        }
        file = root.openNextFile();
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void readFileOverBluetooth(fs::FS &fs, String path, BluetoothSerial &btSerial) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("Transmitting data: ");
  while (file.available()) {
    char data = file.read();
    Serial.write(data);
    btSerial.write(data);
  }
  file.close();
}

void listFiles(fs::FS &fs, const char * dirname, uint8_t levels, BluetoothSerial &btSerial) {
    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            btSerial.print("DIR:");
            btSerial.println(file.name());
            if(levels){
                listFiles(fs, file.path(), levels -1, btSerial);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.println(file.name());
            btSerial.print("FILE:");
            btSerial.println(file.name());
        }
        file = root.openNextFile();
    }
}


void setup() {
  Serial.begin(115200);
  SerialBT.begin(device_name); //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  //Serial.printf("The device with name \"%s\" and MAC address %s is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str(), SerialBT.getMacString()); // Use this after the MAC method is implemented
  #ifdef USE_PIN
    SerialBT.setPin(pin);
    Serial.println("Using PIN");
  #endif

  while (!SD.begin(25)) {
    Serial.println("Card Mount Failed");
    delay(500);
    Serial.println("Retrying.");
  }

  rtc.begin();
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
      Serial.println("No SD card attached");
      return;
  }
  
  listDir(SD, "/", 0);
  readFile(SD, "/data/samples.txt");

  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
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

