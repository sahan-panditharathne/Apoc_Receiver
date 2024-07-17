bool parseSensorData(const String& dataString, SensorData& data) {
  // Check start and end markers
  if (!dataString.startsWith("##") || !dataString.endsWith("$$")) {
    writeToLogs("Message format mismatch");
    return false;
  }

  // Remove start and end markers
  String cleanData = dataString.substring(2, dataString.length() - 2);

  // Split the main sections
  int atSymbols[5];
  int atCount = 0;
  for (int i = 0; i < cleanData.length() && atCount < 5; i++) {
    if (cleanData.charAt(i) == '@') {
      atSymbols[atCount++] = i;
    }
  }
  if (atCount != 5) return false;

  // Extract netId and check if it matches USER_NET_ID
  data.netId = cleanData.substring(0, atSymbols[0]);
  if (data.netId != USER_NET_ID) {
    writeToLogs("Network ID mismatch");
    return false;  // Discard the message if netId doesn't match
  }

  // Extract other main sections
  data.msgType = cleanData.substring(atSymbols[0] + 1, atSymbols[1]);
  String nodeInfo = cleanData.substring(atSymbols[1] + 1, atSymbols[2]);
  String envData = cleanData.substring(atSymbols[2] + 1, atSymbols[3]);
  String powerData = cleanData.substring(atSymbols[3] + 1, atSymbols[4]);
  
  // Parse NODE_INFO
  int colonPos1 = nodeInfo.indexOf(':');
  int colonPos2 = nodeInfo.lastIndexOf(':');
  if (colonPos1 == -1 || colonPos2 == -1 || colonPos1 == colonPos2) return false;
  data.nodeId = nodeInfo.substring(0, colonPos1);
  data.sequence = nodeInfo.substring(colonPos1 + 1, colonPos2).toInt();
  data.timestamp = nodeInfo.substring(colonPos2 + 1).toInt();

  // Parse ENV_DATA
  int semiColons[3];
  int semiCount = 0;
  for (int i = 0; i < envData.length() && semiCount < 3; i++) {
    if (envData.charAt(i) == ';') {
      semiColons[semiCount++] = i;
    }
  }
  if (semiCount != 3) return false;
  data.temperature = envData.substring(0, semiColons[0]).toFloat();
  data.humidity = envData.substring(semiColons[0] + 1, semiColons[1]).toFloat();
  data.light = envData.substring(semiColons[1] + 1, semiColons[2]).toFloat();
  data.soil = envData.substring(semiColons[2] + 1).toFloat();

  // Parse POWER_DATA
  data.batteryVoltage = powerData.toFloat();
  writeToLogs("Parsed successfully");
  return true;
}