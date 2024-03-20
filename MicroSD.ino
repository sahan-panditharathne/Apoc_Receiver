void MicroSD_init(){
  while (!SD.begin(25)) {
  Serial.println("Card Mount Failed");
  delay(500);
  Serial.println("Retrying.");
  }

  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
      Serial.println("No SD card attached");
      return;
  }
  
  listDir(SD, "/", 0);
  readFile(SD, "/data/samples.txt");
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
            Serial.print (file.name());
            time_t t= file.getLastWrite();
            struct tm * tmstruct = localtime(&t);
            Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.print(file.size());
            time_t t= file.getLastWrite();
            struct tm * tmstruct = localtime(&t);
            Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
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

void readFileOverBluetooth(fs::FS &fs, String path, BluetoothSerial &btSerial) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("Transmitting data: ");
  while (file.available()) {
    char data = file.read();
    Serial.write(data);
    btSerial.write(data);
  }
  file.close();
}

void listFiles(fs::FS &fs, const char * dirname, uint8_t levels, BluetoothSerial &btSerial) {
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
            Serial.println(file.name());
            btSerial.print("DIR:");
            btSerial.println(file.name());
            if(levels){
                listFiles(fs, file.path(), levels -1, btSerial);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.println(file.name());
            btSerial.print("FILE:");
            btSerial.println(file.name());
        }
        file = root.openNextFile();
    }
}
