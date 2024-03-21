void RTC_init(RTC_DS1307 &rtc){
  rtc.begin();
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}

String Todayfilepath(RTC_DS1307 &rtc){
  DateTime now = rtc.now();
  
  // Format the date as DD-MM-YYYY
  String day = (now.day() < 10) ? "0" + String(now.day()) : String(now.day());
  String month = (now.month() < 10) ? "0" + String(now.month()) : String(now.month());
  String year = String(now.year());

  return day + "-" + month + "-" + year;
}

String UnixTime(RTC_DS1307 &rtc){
  DateTime now = rtc.now();
  return String(now.unixtime());
}