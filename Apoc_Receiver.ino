#include <SPI.h>
#include "FS.h"
#include "RTClib.h"
#include <LoRa.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

struct SensorData {
  char networkID[5];
  char nodeID[5];
  unsigned long sequence;
  unsigned long timestamp;
  float temperature;
  float humidity;
  float light;
  float soilMoisture;
  float batteryVoltage;
  char checksum[5];
};

RTC_DS1307 rtc;

void setup() {
  Serial.begin(115200);

  LoRa_init();
  Flash_Init();
  RTC_init(rtc);
  BLE_init();

  // Create FreeRTOS tasks for LoRa and BLE services
  xTaskCreate(LoRa_services, "LoRa Task", 8192, NULL, 2, NULL);
  xTaskCreate(BLE_services, "BLE Task", 8192, NULL, 1, NULL);
}

void loop() {

}