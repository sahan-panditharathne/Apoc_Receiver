#include "SPIFFS.h"

#ifndef ULLONG_MAX
#define ULLONG_MAX 18446744073709551615ULL
#endif

void Flash_Init() {
  while (!SPIFFS.begin(25)) {
    Serial.println("SPIFFS Mount Failed");
    delay(500);
    Serial.println("Retrying.");
    indicateError();
  }
  
  listDir(SPIFFS, "/", 0);

  Serial.println("Looking for data");
  String latestDataFile = "/data/"+findLatestDateFile("/data");
  if (latestDataFile.length() > 0) {
      Serial.print("Latest data file: ");
      Serial.println(latestDataFile);
      readFile(SPIFFS, latestDataFile.c_str());
  } else {
      Serial.println("No files found.");
  }

  Serial.println("Looking for logs");
  String latestLogFile = "/logs/"+findLatestDateFile("/logs");
  if (latestLogFile.length() > 0) {
      Serial.print("Latest log file: ");
      Serial.println(latestLogFile);
      readFile(SPIFFS, latestLogFile.c_str());
  } else {
      Serial.println("No files found.");
  }
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.print(file.name());
            time_t t = file.getLastWrite();
            struct tm * tmstruct = localtime(&t);
            Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
            if(levels){
                listDir(fs, file.path(), levels - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.print(file.size());
            time_t t = file.getLastWrite();
            struct tm * tmstruct = localtime(&t);
            Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
        }
        file = root.openNextFile();
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

// Function to find the latest date file
String findLatestDateFile(const char* dirname) {
    File root = SPIFFS.open(dirname);
    if (!root || !root.isDirectory()) {
        Serial.println("Failed to open directory or directory not found.");
        return "";
    }

    File file = root.openNextFile();
    String latestDateFile = "";
    uint64_t latestTimestamp = 0;

    while (file) {
        String fileName = file.name();

        // Extract the Unix timestamp from the file name
        uint64_t fileTimestamp = strtoull(fileName.c_str(), NULL, 10);

        if (fileTimestamp > latestTimestamp) {
            latestTimestamp = fileTimestamp;
            latestDateFile = fileName;
        }

        file = root.openNextFile();
    }

    root.close();
    return latestDateFile;
}

// Function to find the oldest date file
String findOldestDateFile(const char* dirname) {
    File root = SPIFFS.open(dirname);
    if (!root || !root.isDirectory()) {
        Serial.println("Failed to open directory or directory not found.");
        return "";
    }

    File file = root.openNextFile();
    String oldestDateFile = "";
    uint64_t oldestTimestamp = ULLONG_MAX;

    while (file) {
        String fileName = file.name();

        // Extract the Unix timestamp from the file name
        uint64_t fileTimestamp = strtoull(fileName.c_str(), NULL, 10);

        if (fileTimestamp < oldestTimestamp) {
            oldestTimestamp = fileTimestamp;
            oldestDateFile = fileName;
        }

        file = root.openNextFile();
    }

    root.close();
    return oldestDateFile;
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

