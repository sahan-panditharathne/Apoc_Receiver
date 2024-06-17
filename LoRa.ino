//define the pins used by the LoRa transceiver module
#define ss 5
#define rst 14
#define dio0 4
#define PW "0001"

void LoRa_init(){
  Serial.println("Starting LoRa Receiver");
  LoRa.setPins(ss, rst, dio0); //setup LoRa transceiver module

  while (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    delay(500);
    Serial.println("Retrying.");
  }
}

void LoRa_services(){
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.println("Packet received");
    // read packet into a buffer
    char buffer[packetSize];
    int i = 0;
    while (LoRa.available()) {
      buffer[i++] = (char)LoRa.read();
    }
    buffer[i] = '\0'; // Null-terminate the buffer

    // Check if the first value matches the expected value
    char *ptr = strtok(buffer, ",");
    if (ptr == NULL || strcmp(ptr, PW) != 0) {
      // First value does not match expected value, ignore the packet
      Serial.println("Ignoring packet");
      return;
    }

    // split the buffer into individual values
    int count = 0;
    String id; //pw is the network password
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

      while (ptr != NULL) { //this is put here tm=emparailry for parse messages coming from 0001
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

    String filePath = "/data/"+Todayfilepath(rtc)+".txt";
    String unixtime = UnixTime(rtc);
    
    // Calculate the checksum of the received message
    float calculatedChecksum = battery + temperature + humidity + lux + soil;

    // Check if the calculated checksum matches the received checksum
    if (/*calculatedChecksum == checksum*/ 1) {
      // Message is likely intact, process it
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

      String message = id+","+String(battery)+","+String(temperature)+","+String(humidity)+","+String(lux)+","+String(soil)+","+unixtime+"\n";
      Serial.println(message);
      appendFile(SPIFFS,filePath.c_str(), message);
    } else {
      // Checksum mismatch, discard the message
      Serial.println("Checksum mismatch, discarding message");
      /*-------Write the errors into a seperate file on the SD card-------*/
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}