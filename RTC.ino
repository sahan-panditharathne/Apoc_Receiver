void RTC_init(RTC_DS1307 &rtc) {
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        return;
    }

    if (!rtc.isrunning()) {
        Serial.println("RTC is NOT running, setting time!");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    } else {
        DateTime now = rtc.now();
        DateTime compileTime = DateTime(F(__DATE__), F(__TIME__));
        
        // If RTC is more than 1 minute off from compile time, update it
        if (abs((long)now.unixtime() - (long)compileTime.unixtime()) > 60) {
            Serial.println("RTC time is significantly off, updating...");
            rtc.adjust(compileTime);
        }
    }

    DateTime now = rtc.now();

    // Set system time
    struct timeval tv;
    tv.tv_sec = now.unixtime();
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);
    
    Serial.printf("Current time: %04d/%02d/%02d %02d:%02d:%02d\n", 
                  now.year(), now.month(), now.day(),
                  now.hour(), now.minute(), now.second());
}

String Todayfilepath(RTC_DS1307 &rtc){
  DateTime now = rtc.now();
  
  // Format the date as DD-MM-YYYY
  String day = (now.day() < 10) ? "0" + String(now.day()) : String(now.day());
  String month = (now.month() < 10) ? "0" + String(now.month()) : String(now.month());
  String year = String(now.year());

  return day + "-" + month + "-" + year;
}

unsigned long TodayUnixTime(RTC_DS1307 &rtc) {
  DateTime now = rtc.now();
  DateTime startOfDay(now.year(), now.month(), now.day(), 0, 0, 0);
  return startOfDay.unixtime();
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