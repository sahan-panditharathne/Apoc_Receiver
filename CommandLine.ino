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
  } else {
    Serial.println("Unknown command");
  }
}

void changeDirectory(String directory) {
  // You can implement a directory handling mechanism here if needed
  // For simplicity, let's just list files in the new directory
  listDir(SPIFFS,directory.c_str(),0);
}