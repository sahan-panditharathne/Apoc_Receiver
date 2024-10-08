#include <SPI.h>
#include "FS.h"
#include "RTClib.h"
#include <LoRa.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

const char* NETWORK_ID = "WD"; //Network identifier for the sensor group - MUST BE CHANGED BY USER
#define  LOGGING 1 //Changing this to 0 will stop the system from recording logs
#define LORA_FREQUENCY 433E6 //LoRa radio frequency in Hz (433 MHz in this case)

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

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  LoRa_init();
  Flash_Init();
  RTC_init(rtc);
  BLE_init();

  // Create FreeRTOS tasks for LoRa and BLE services
  xTaskCreate(LoRa_services, "LoRa Task", 8192, NULL, 2, NULL);
  xTaskCreate(BLE_services, "BLE Task", 8192, NULL, 1, NULL);

  writeToLogs("System booted up");
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    handleCommand(command);
  }
}

void indicateError() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}