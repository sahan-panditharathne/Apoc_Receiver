void writeToLogs(String message){
  String filePath = "/logs/" + TodayUnixTime(rtc);
  String log = getCurrentTime24H(rtc)+" "+ message+"\n";
  appendFile(SPIFFS, filePath.c_str(), log.c_str());
}