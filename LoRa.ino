#define ss 5
#define rst 14
#define dio0 4
#define PW "WD"
#define MAX_MESSAGE_LENGTH 256

void LoRa_init() {
  Serial.println("Starting LoRa Receiver");
  LoRa.setPins(ss, rst, dio0); //setup LoRa transceiver module

  while (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    delay(500);
    Serial.println("Retrying.");
    indicateError();
  }
}

void LoRa_services(void *pvParameters) {
  for (;;) {
    // Try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String receivedMessage = "";
    while (LoRa.available()) {
      receivedMessage += (char)LoRa.read();
    }
    Serial.print("Received message: ");
    Serial.println(receivedMessage);

    // Extract the network ID from the message
    char buffer[100];
    strcpy(buffer, receivedMessage.c_str());
    char* start = strstr(buffer, "##");
    char* end = strstr(buffer, "@");
    if (start != nullptr && end != nullptr && end > start + 2) {
      char receivedNetworkID[10];
      strncpy(receivedNetworkID, start + 2, end - start - 2);
      receivedNetworkID[end - start - 2] = '\0';

      // Check if the network ID matches the desired network ID
      if (strcmp(receivedNetworkID, NETWORK_ID) == 0) {
        // Extract the part of the message to calculate checksum
        char* checksumStart = start + 2;
        char* checksumEnd = strstr(checksumStart, "@");
        if (checksumEnd != nullptr) {
          *checksumEnd = '\0';

          // Calculate the checksum of the message
          unsigned int calculatedChecksum = calculateChecksum(checksumStart);
          *checksumEnd = '@';

          // Parse the received message
          SensorData data = parseLoRaMessage(receivedMessage.c_str());

          // Verify checksum
          unsigned int receivedChecksum = strtol(data.checksum, nullptr, 16);
          if (/*calculatedChecksum == receivedChecksum*/ 1) {
            // Output the parsed data
            Serial.print("Network ID: "); Serial.println(data.networkID);
            Serial.print("Node ID: "); Serial.println(data.nodeID);
            Serial.print("Sequence: "); Serial.println(data.sequence);
            Serial.print("Timestamp: "); Serial.println(data.timestamp);
            Serial.print("Temperature: "); Serial.println(data.temperature);
            Serial.print("Humidity: "); Serial.println(data.humidity);
            Serial.print("Light: "); Serial.println(data.light);
            Serial.print("Soil Moisture: "); Serial.println(data.soilMoisture);
            Serial.print("Battery Voltage: "); Serial.println(data.batteryVoltage);
            Serial.print("Checksum: "); Serial.println(data.checksum);

            String unixtime = UnixTime(rtc);
            String message = String(data.nodeID) + "," + String(data.batteryVoltage) + "," + String(data.temperature) + "," + String(data.humidity) + "," + String(data.light) + "," + String(data.soilMoisture) + "," + unixtime + "\n";
            String filePath = "/data/" + TodayUnixTime(rtc);
            appendFile(SPIFFS, filePath.c_str(), message.c_str()); //write to storage

            writeToLogs("msg saved, nodeid:"+String(data.nodeID)+", sequence:"+String(data.sequence)+",waketime:"+String(data.timestamp)+", RSSI:"+LoRa.packetRssi());
          } else {
            Serial.println("Message discarded due to checksum mismatch.");
            writeToLogs("Message discarded. checksum mismatch.");
          }
        } else {
          Serial.println("Invalid message format for checksum extraction.");
          writeToLogs("Message discarded. Invalid message format.");
        }
      } else {
        Serial.println("Message discarded due to unmatched network ID.");
        writeToLogs("Message discarded. unmatched network ID.");
      }
    } else {
      Serial.println("Invalid message format.");
      writeToLogs("Message discarded. Invalid message format.");
    }
  }

    vTaskDelay(10 / portTICK_PERIOD_MS); // Yield to other tasks
  }
}