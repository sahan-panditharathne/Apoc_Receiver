#include <SPI.h>
#include "FS.h"
#include "SD.h"
#include "BluetoothSerial.h"
#include "RTClib.h"
#include <LoRa.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

RTC_DS1307 rtc;
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);

  Bluetooth_init(SerialBT);
  LoRa_init();
  Flash_Init();
  RTC_init(rtc);
  BLE_init();
}

void loop() {
  LoRa_services();
  Bluetooth_services(SerialBT);
  BLE_Services();
}

