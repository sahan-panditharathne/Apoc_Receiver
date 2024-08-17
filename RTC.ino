void RTC_init(RTC_DS1307 &rtc) {
    while(!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        delay(500);
        indicateError();
    }
    
    DateTime now = rtc.now();
    setSystemTime(now);
}

String Todayfilepath(RTC_DS1307 &rtc){
  DateTime now = rtc.now();
  
  // Format the date as DD-MM-YYYY
  String day = (now.day() < 10) ? "0" + String(now.day()) : String(now.day());
  String month = (now.month() < 10) ? "0" + String(now.month()) : String(now.month());
  String year = String(now.year());

  return day + "-" + month + "-" + year;
}

String TodayUnixTime(RTC_DS1307 &rtc) {
  DateTime now = rtc.now();
  DateTime startOfDay(now.year(), now.month(), now.day(), 0, 0, 0);
  return String(startOfDay.unixtime());
}

String getCurrentTime24H(RTC_DS1307 &rtc) {
  DateTime now = rtc.now();
  
  // Format the time as HH:MM:SS
  String hour = (now.hour() < 10) ? "0" + String(now.hour()) : String(now.hour());
  String minute = (now.minute() < 10) ? "0" + String(now.minute()) : String(now.minute());
  String second = (now.second() < 10) ? "0" + String(now.second()) : String(now.second());

  return hour + ":" + minute + ":" + second;
}

String UnixTime(RTC_DS1307 &rtc){
  DateTime now = rtc.now();
  return String(now.unixtime() * 1000ULL);
}

void displayDateTime() {
  DateTime now = rtc.now();
  
  Serial.print("Current Date: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" Time: ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
}

void setTime(String timeStr) {
  int hour = timeStr.substring(0, 2).toInt();
  int minute = timeStr.substring(3, 5).toInt();
  int second = timeStr.substring(6, 8).toInt();
  
  DateTime now = rtc.now();
  rtc.adjust(DateTime(now.year(), now.month(), now.day(), hour, minute, second));
  Serial.println("Time set successfully");

}

void setDate(String dateStr) {
  int year = dateStr.substring(0, 4).toInt();
  int month = dateStr.substring(5, 7).toInt();
  int day = dateStr.substring(8, 10).toInt();
  
  DateTime now = rtc.now();
  rtc.adjust(DateTime(year, month, day, now.hour(), now.minute(), now.second()));
  Serial.println("Date set successfully");
  now = rtc.now();
  setSystemTime(now);
}

void setSystemTime(DateTime now){
  // Set system time
    struct timeval tv;
    tv.tv_sec = now.unixtime();
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);

    Serial.printf("Current time: %04d/%02d/%02d %02d:%02d:%02d\n", 
                  now.year(), now.month(), now.day(),
                  now.hour(), now.minute(), now.second());
}
