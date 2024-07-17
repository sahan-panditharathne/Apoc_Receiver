#include "SPIFFS.h"

BLEServer* pServer = NULL;
BLECharacteristic* pcLatestDate = NULL;
BLEDescriptor *pDescr;

BLECharacteristic* pcSync = NULL;
BLEDescriptor *pDescrSync;

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

#define SERVICE_UUID        "8292fed4-e037-4dd7-b0c8-c8d7c80feaae"
#define CHARACTERISTIC_UUID "d5641f9f-1d6f-4f8e-94db-49fbfe9192ab"
#define CHARACTERISTIC_UUID2 "870e104f-ba63-4de3-a3ac-106969eac292"

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Device connected");
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device disconnected");
  }
};

class LatestDate : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pcLatestDate) {
    pcLatestDate->setValue(findLatestDateFile("/data").c_str());
  }
};

class syncData: public BLECharacteristicCallbacks {
  File dataFile;
  String fileName = findLatestDateFile("/data");
  String filePath = "/data/"+fileName;
  bool open = 0;

  void onWrite(BLECharacteristic *pCharacteristic) {
    fileName = "/" + pCharacteristic->getValue();
    filePath = "/data/"+fileName;
    Serial.println("onWrite()");
  }

  void onRead(BLECharacteristic *pcSync) {
    if (!open) {
      dataFile = SPIFFS.open(filePath.c_str());
      if (!dataFile) {
        Serial.println("Could not open file for reading!");
        pcSync->setValue("failed");
        return;
      } else {
        pcSync->setValue("ok");
        open = 1;
      }
    } else {
      if (dataFile.available()) {
        String value = dataFile.readStringUntil('\n');
        pcSync->setValue(value.c_str());
      } else {
        pcSync->setValue("end");
        dataFile.close();
        open = 0;
        Serial.println("File has been read.");
      }
    }
  }
};

void BLE_init() {
  BLEDevice::init("APOC RECEIVER");

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pcLatestDate = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ
  );

  pcSync = pService->createCharacteristic(
    CHARACTERISTIC_UUID2,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE
  );

  pcLatestDate->addDescriptor(new BLE2902());

  pDescr = new BLEDescriptor((uint16_t)0x2901);
  pDescr->setValue("latest date");
  pcLatestDate->addDescriptor(pDescr);

  pcSync->addDescriptor(new BLE2902());

  pDescrSync = new BLEDescriptor((uint16_t)0x2901);
  pDescrSync->setValue("sync data");
  pcSync->addDescriptor(pDescrSync);

  pcLatestDate->setCallbacks(new LatestDate());
  pcSync->setCallbacks(new syncData());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
  Serial.println("Waiting for a client connection to notify...");
}

void BLE_services(void *pvParameters) {
  for (;;) {
    if (deviceConnected) {
      pcLatestDate->setValue((uint8_t*)&value, 4);
      pcLatestDate->notify();
      value++;
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    if (!deviceConnected && oldDeviceConnected) {
      vTaskDelay(500 / portTICK_PERIOD_MS);
      pServer->startAdvertising();
      Serial.println("Start advertising");
      oldDeviceConnected = deviceConnected;
    }
    if (deviceConnected && !oldDeviceConnected) {
      oldDeviceConnected = deviceConnected;
    }

    vTaskDelay(10 / portTICK_PERIOD_MS); // Yield to other tasks
  }
}
