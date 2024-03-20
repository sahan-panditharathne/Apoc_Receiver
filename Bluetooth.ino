void Bluetooth_init(){
  SerialBT.begin(device_name); //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  //Serial.printf("The device with name \"%s\" and MAC address %s is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str(), SerialBT.getMacString()); // Use this after the MAC method is implemented
  #ifdef USE_PIN
    SerialBT.setPin(pin);
    Serial.println("Using PIN");
  #endif
}

void Bluetooth_services(){

  if (SerialBT.available()) {
    // Read incoming command
    String command = SerialBT.readStringUntil('\n');
    Serial.print("Received command: ");
    Serial.println(command);

    // Check if the command starts with "SYNC"
    if (command.startsWith("SYNC")) {
      // Extract the date from the command
      String date = command.substring(5);
      date.trim(); // Remove leading/trailing whitespaces

      // Construct the file path based on the date
      String filePath = "/data/" + date + ".txt";
      
      // Check if the file exists
      if (SD.exists(filePath)) {
        // Read and transmit the file content over Bluetooth
        readFileOverBluetooth(SD, filePath, SerialBT);
      } else {
        Serial.println("File not found");
        SerialBT.println("ERROR: File not found");
      }
    }else if (command.startsWith("LIST")) {
      /*
      Retrives the last N number of 
      expected command syntax LIST N
      N - number of files - integer
      */
      int count = command.substring(5).toInt();
      listFiles(SD, "/data", count, SerialBT);

    }else if (command.startsWith("SETTIME")) {
      /*
      Sets the RTC time
      expected command syntax SETTIME unixtimestamp
      */
      unsigned long unixtime = command.substring(8).toInt();
      rtc.adjust(DateTime(unixtime));
      Serial.println("RTC time set");
      SerialBT.println("SUCCESS: time set!");

    } else {
        SerialBT.println("Invalid command");
    }
  }
  delay(20);
  
}