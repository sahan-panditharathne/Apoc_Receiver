#define ss 5
#define rst 14
#define dio0 4
#define PW "0001"
const String USER_NET_ID = "WD";

void LoRa_init() {
  Serial.println("Starting LoRa Receiver");
  LoRa.setPins(ss, rst, dio0); //setup LoRa transceiver module

  while (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    delay(500);
    Serial.println("Retrying.");
  }
}

void LoRa_services(void *pvParameters) {
  for (;;) {
    // Try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Received a packet
    Serial.println("Received a packet");

    String receivedData = "";
    // Read packet
    while (LoRa.available()) {
      receivedData += (char)LoRa.read();
    }

    // Parse the received data
    SensorData parsedData;
    if (parseSensorData(receivedData, parsedData)) {
      Serial.println("Parsing successful! Message matches our Network ID.");
      Serial.print("Received packet '");
      Serial.print(receivedData);
      Serial.print("' with RSSI ");
      Serial.println(LoRa.packetRssi());
      
      Serial.print("Network ID: ");
      Serial.println(parsedData.netId);
      Serial.print("Message Type: ");
      Serial.println(parsedData.msgType);
      Serial.print("Node ID: ");
      Serial.println(parsedData.nodeId);
      Serial.print("Sequence: ");
      Serial.println(parsedData.sequence);
      Serial.print("Timestamp: ");
      Serial.println(parsedData.timestamp);
      Serial.print("Temperature: ");
      Serial.println(parsedData.temperature);
      Serial.print("Humidity: ");
      Serial.println(parsedData.humidity);
      Serial.print("Light: ");
      Serial.println(parsedData.light);
      Serial.print("Soil: ");
      Serial.println(parsedData.soil);
      Serial.print("Battery Voltage: ");
      Serial.println(parsedData.batteryVoltage);

      String filePath = "/data/" + TodayUnixTime(rtc);
      String unixtime = UnixTime(rtc);
      String message = parsedData.netId + "," + parsedData.batteryVoltage + "," + parsedData.temperature + "," + parsedData.humidity + "," + parsedData.light + "," + parsedData.soil + "," + unixtime + "\n";

      appendFile(SPIFFS, filePath.c_str(), message); //write to storage

      writeToLogs("msg saved, nodeid:"+parsedData.nodeId+", sequence:"+parsedData.sequence+",waketime:"+parsedData.timestamp+", RSSI:"+LoRa.packetRssi());
    } else {
      Serial.println("Parsing failed. Discarding.");
      writeToLogs("Parsing failed. Discarding.");
    }
  }

    vTaskDelay(10 / portTICK_PERIOD_MS); // Yield to other tasks
  }
}
