void handleCommand(String command) {
  command.trim(); // Remove any leading/trailing whitespace
  
  if (command == "LS") {
    listDir(SPIFFS,"/",0);
  } else if (command.startsWith("READ ")) {
    String fileName = command.substring(5);
    readFile(SPIFFS,fileName.c_str());
  } else if (command.startsWith("CD ")) {
    String directory = command.substring(3);
    changeDirectory(directory);
  } else if (command.startsWith("DEL ")) {
    String directory = command.substring(4);
    deleteFile(SPIFFS, directory.c_str());
  } else if (command.startsWith("SETTIME ")) {
    setTime(command.substring(8));
  } else if (command.startsWith("SETDATE ")) {
    setDate(command.substring(8));
  } else if (command == "DATETIME") {
    displayDateTime();
  } else if (command == "FLASHINFO") {
    displayFlashInfo();
  } else if (command == "USAGE") {
    displayFlashUsage();
  } else if (command == "HELP") {
    displayCommandCatalog();
  } else {
    Serial.println("Unknown command.");
    displayCommandCatalog();
  }
}

void changeDirectory(String directory) {
  // we can implement a directory handling mechanism here if needed
  // For simplicity, let's just list files in the new directory
  listDir(SPIFFS,directory.c_str(),0);
}

void displayFlashInfo() {
  size_t totalBytes = SPIFFS.totalBytes();
  size_t usedBytes = SPIFFS.usedBytes();
  
  Serial.println("SPI Flash Info:");
  Serial.printf("Total Capacity: %u bytes\n", totalBytes);
  Serial.printf("Used Capacity: %u bytes\n", usedBytes);
}

void displayFlashUsage() {
  size_t totalBytes = SPIFFS.totalBytes();
  size_t usedBytes = SPIFFS.usedBytes();
  float usedPercentage = (usedBytes * 100.0) / totalBytes;
  
  Serial.println("SPI Flash Usage:");
  Serial.printf("Used: %u bytes (%.2f%% of total capacity)\n", usedBytes, usedPercentage);
}

void displayCommandCatalog() {
  Serial.println("Command Catalog:");
  Serial.println("LS          : List all files and directories in the current directory");
  Serial.println("READ <file> : Read the specified file");
  Serial.println("CD <dir>    : Change to the specified directory");
  Serial.println("FLASHINFO   : Show total and used capacity of SPI flash");
  Serial.println("USAGE       : Show used capacity in bytes and as a percentage");
  Serial.println("SETTIME <HH:MM:SS> : Set the RTC time");
  Serial.println("SETDATE <YYYY-MM-DD> : Set the RTC date");
  Serial.println("DATETIME    : Display the current date and time");
  Serial.println("HELP        : Show this command catalog");
}