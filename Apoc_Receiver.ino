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
#include <LoRa.h>

RTC_DS1307 rtc;
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);

  Bluetooth_init(SerialBT);
  LoRa_init();
  MicroSD_init();
  RTC_init(rtc);

}

void loop() {
  LoRa_services();
  Bluetooth_services(SerialBT);
}

