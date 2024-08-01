SensorData parseLoRaMessage(const char* message) {
  SensorData data;

  // Create temporary buffer to work with
  char buffer[100];
  strcpy(buffer, message);

  // Remove start and end delimiters
  char* start = strstr(buffer, "##");
  char* end = strstr(buffer, "$$");
  if (start == nullptr || end == nullptr) {
    return data;  // Invalid message format
  }
  *end = '\0';  // Null terminate the message at the end delimiter

  // Move past the start delimiter
  start += 2;

  // Tokenize the message using '@' as the delimiter
  char* token = strtok(start, "@");

  // Extract network ID
  strncpy(data.networkID, token, sizeof(data.networkID) - 1);
  data.networkID[sizeof(data.networkID) - 1] = '\0';

  // Move to the next token (ENVSENSOR)
  token = strtok(nullptr, "@");

  // Extract node ID, sequence number, and timestamp
  token = strtok(nullptr, "@");
  sscanf(token, "%[^:]:%lu:%lu", data.nodeID, &data.sequence, &data.timestamp);

  // Extract sensor readings
  token = strtok(nullptr, "@");
  sscanf(token, "%f;%f;%f;%f", &data.temperature, &data.humidity, &data.light, &data.soilMoisture);

  // Extract battery voltage
  token = strtok(nullptr, "@");
  data.batteryVoltage = atof(token);

  // Extract checksum
  token = strtok(nullptr, "@");
  strncpy(data.checksum, token, sizeof(data.checksum) - 1);
  data.checksum[sizeof(data.checksum) - 1] = '\0';

  return data;
}

unsigned int calculateChecksum(const char* message) {
  unsigned int checksum = 0;
  while (*message) {
    checksum += *message++;
  }
  return checksum;
}