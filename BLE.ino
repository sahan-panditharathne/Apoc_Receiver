#include "SPIFFS.h"
BLEServer* pServer = NULL;
BLECharacteristic* pcLatestDate = NULL;
BLEDescriptor *pDescr;

BLECharacteristic* pcSync = NULL;
BLEDescriptor *pDescrSync;

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "8292fed4-e037-4dd7-b0c8-c8d7c80feaae"
#define CHARACTERISTIC_UUID "d5641f9f-1d6f-4f8e-94db-49fbfe9192ab"
#define CHARACTERISTIC_UUID2 "870e104f-ba63-4de3-a3ac-106969eac292"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class LatestDate : public BLECharacteristicCallbacks {
  String getLatestDate() {
    String latestDate;
    File root = SPIFFS.open("/");
    File file = root.openNextFile();

    while (file) {
      if (file.isDirectory()) {
        file = root.openNextFile();
        continue;
      }
      
      String fileName = file.name();
      if (fileName.endsWith(".txt")) {
        String fileDate = fileName.substring(0, fileName.indexOf(".txt"));
        if (latestDate.length() == 0 || fileDate > latestDate) {
          latestDate = fileDate;
        }
      }

      file = root.openNextFile();
    }

    root.close();
    return latestDate;
  }

  void onRead(BLECharacteristic *pcLatestDate) {
    // Implement the logic to send data over BLE
    pcLatestDate->setValue(getLatestDate().c_str());
  }
};

class syncData: public BLECharacteristicCallbacks {
  File dataFile;
  String fileName = "/data.txt";
  bool open = 0;

  void onWrite(BLECharacteristic *pCharacteristic) {
    fileName = pCharacteristic->getValue();
    Serial.println("onWrite()");
  }

  void onRead(BLECharacteristic *pcSync) {
    //Serial.println("onRead()");
    // Implement the logic to send data over BLE
    if (!open) {
      dataFile = SPIFFS.open(fileName.c_str());
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
        pcSync->setValue(value.c_str()); // Read a line from the file
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
    // Create the BLE Device
    BLEDevice::init("ESP32");

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pcLatestDate = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ
    );

    pcSync = pService->createCharacteristic(
        CHARACTERISTIC_UUID2,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE
    );

    // Create a BLE Descriptor for latest date
    pcLatestDate->addDescriptor(new BLE2902());

    pDescr = new BLEDescriptor((uint16_t)0x2901);
    pDescr->setValue("latest date");
    pcLatestDate->addDescriptor(pDescr);

    // Create a BLE Descriptor for sync data
    pcSync->addDescriptor(new BLE2902());

    pDescrSync = new BLEDescriptor((uint16_t)0x2901);
    pDescrSync->setValue("sync data");
    pcSync->addDescriptor(pDescrSync);

    // Set callback
    pcLatestDate->setCallbacks(new LatestDate());
    pcSync->setCallbacks(new syncData());

    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
    Serial.println("Waiting for a client connection to notify...");
}

void BLE_Services() {
    // Notify changed value
    if (deviceConnected) {
        pcLatestDate->setValue((uint8_t*)&value, 4);
        pcLatestDate->notify();
        value++;
        delay(1000); // bluetooth stack will go into congestion if too many packets are sent. In a 6-hour test, I was able to go as low as 3ms
    }
    // Disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("Start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // Connecting
    if (deviceConnected && !oldDeviceConnected) {
        // Do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
