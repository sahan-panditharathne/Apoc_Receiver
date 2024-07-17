#define ss 5
#define rst 14
#define dio0 4
#define PW "0001"

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
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      Serial.println("Packet received");
      char buffer[packetSize];
      int i = 0;
      while (LoRa.available()) {
        buffer[i++] = (char)LoRa.read();
      }
      buffer[i] = '\0'; // Null-terminate the buffer

      char *ptr = strtok(buffer, ",");
      if (ptr == NULL || strcmp(ptr, PW) != 0) {
        Serial.println("Ignoring packet");
        continue;
      }

      int count = 0;
      String id;
      int battery;
      float checksum, temperature, humidity, lux, soil;

      /*while (ptr != NULL) { 
      switch (count) {
        case 0:
          break;
        case 1:
          id = String(ptr);
          break;
        case 2:
          battery = atoi(ptr);
          break;
        case 3:
          checksum = atof(ptr);
          break;
        case 4:
          temperature = atof(ptr);
          break;
        case 5:
          humidity = atof(ptr);
          break;
        case 6:
          lux = atof(ptr);
          break;
        case 7:
          soil = atof(ptr);
          break;
        default:
          break;
      }*/

      while (ptr != NULL) {
        switch (count) {
          case 0:
            id = String(ptr);
            break;
          case 1:
            battery = atoi(ptr);
            break;
          case 2:
            checksum = atof(ptr);
            break;
          case 3:
            temperature = atof(ptr);
            break;
          case 4:
            humidity = atof(ptr);
            break;
          case 5:
            lux = atof(ptr);
            break;
          case 6:
            soil = atof(ptr);
            break;
          default:
            break;
        }

        ptr = strtok(NULL, ",");
        count++;
      }

      String filePath = "/data/" + TodayUnixTime(rtc);
      String unixtime = UnixTime(rtc);

      float calculatedChecksum = battery + temperature + humidity + lux + soil;

      if (/*calculatedChecksum == checksum*/ 1) {
        Serial.println("Message received successfully:");
        Serial.print("ID: ");
        Serial.println(id);
        Serial.print("Battery: ");
        Serial.println(battery);
        Serial.print("Temperature: ");
        Serial.println(temperature);
        Serial.print("Humidity: ");
        Serial.println(humidity);
        Serial.print("Lux: ");
        Serial.println(lux);
        Serial.print("Soil: ");
        Serial.println(soil);

        String message = id + "," + String(battery) + "," + String(temperature) + "," + String(humidity) + "," + String(lux) + "," + String(soil) + "," + unixtime + "\n";
        Serial.println(message);
        appendFile(SPIFFS, filePath.c_str(), message);
      } else {
        Serial.println("Checksum mismatch, discarding message");
      }

      Serial.print(" with RSSI ");
      Serial.println(LoRa.packetRssi());
    }

    vTaskDelay(10 / portTICK_PERIOD_MS); // Yield to other tasks
  }
}
