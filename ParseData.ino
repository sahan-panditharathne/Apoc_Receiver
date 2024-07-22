bool parseSensorData(char* dataString, SensorData* data) {
  char* token;
  char* rest = dataString;

  // Check start and end markers
  if (strncmp(dataString, "##", 2) != 0 || dataString[strlen(dataString)-2] != '$' || dataString[strlen(dataString)-1] != '$') {
    return false;
  }

  // Remove start and end markers
  memmove(dataString, dataString + 2, strlen(dataString) - 3);
  dataString[strlen(dataString) - 3] = '\0';

  // Extract netId
  token = strtok_r(rest, "@", &rest);
  if (!token) return false;
  strncpy(data->netId, token, sizeof(data->netId) - 1);
  data->netId[sizeof(data->netId) - 1] = '\0';

  // Check if netId matches
  if (strcmp(data->netId, USER_NET_ID.c_str()) != 0) {
    return false;
  }

  // Extract msgType
  token = strtok_r(rest, "@", &rest);
  if (!token) return false;
  strncpy(data->msgType, token, sizeof(data->msgType) - 1);
  data->msgType[sizeof(data->msgType) - 1] = '\0';

  // Extract nodeInfo
  token = strtok_r(rest, "@", &rest);
  if (!token) return false;
  char* nodeInfo = token;

  // Parse nodeInfo
  char* nodeInfoRest = nodeInfo;
  token = strtok_r(nodeInfoRest, ":", &nodeInfoRest);
  if (!token) return false;
  strncpy(data->nodeId, token, sizeof(data->nodeId) - 1);
  data->nodeId[sizeof(data->nodeId) - 1] = '\0';

  token = strtok_r(nodeInfoRest, ":", &nodeInfoRest);
  if (!token) return false;
  data->sequence = atoi(token);

  token = strtok_r(nodeInfoRest, ":", &nodeInfoRest);
  if (!token) return false;
  data->timestamp = strtoul(token, NULL, 10);

  // Extract envData
  token = strtok_r(rest, "@", &rest);
  if (!token) return false;
  char* envData = token;

  // Parse envData
  char* envDataRest = envData;
  token = strtok_r(envDataRest, ";", &envDataRest);
  if (!token) return false;
  data->temperature = atof(token);

  token = strtok_r(envDataRest, ";", &envDataRest);
  if (!token) return false;
  data->humidity = atof(token);

  token = strtok_r(envDataRest, ";", &envDataRest);
  if (!token) return false;
  data->light = atof(token);

  token = strtok_r(envDataRest, ";", &envDataRest);
  if (!token) return false;
  data->soil = atof(token);

  // Extract powerData
  token = strtok_r(rest, "@", &rest);
  if (!token) return false;
  data->batteryVoltage = atof(token);

  return true;
}
