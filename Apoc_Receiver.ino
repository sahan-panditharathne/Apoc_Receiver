#include <SPI.h>
#include "FS.h"
#include "BluetoothSerial.h"
#include "RTClib.h"
#include <LoRa.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

RTC_DS1307 rtc;

void setup() {
  Serial.begin(115200);

  LoRa_init();
  Flash_Init();
  RTC_init(rtc);
  BLE_init();

  // Create FreeRTOS tasks for LoRa and BLE services
  xTaskCreate(LoRa_services, "LoRa Task", 4096, NULL, 1, NULL);
  xTaskCreate(BLE_services, "BLE Task", 16384, NULL, 1, NULL);
}

void loop() {

}