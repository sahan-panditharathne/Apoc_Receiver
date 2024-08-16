void RTC_init(RTC_DS1307 &rtc) {
    while(!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        delay(500);
        indicateError();
    }

    // if (!rtc.isrunning()) {
    //     Serial.println("RTC is NOT running, setting time!");
    //     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // } else {
    //     DateTime now = rtc.now();
    //     DateTime compileTime = DateTime(F(__DATE__), F(__TIME__));
        
    //     // If RTC is more than 1 minute off from compile time, update it
    //     if (abs((long)now.unixtime() - (long)compileTime.unixtime()) > 60) {
    //         Serial.println("RTC time is significantly off, updating...");
    //         rtc.adjust(compileTime);
    //     }
    // }

    // if (rtc.lostPower()) {
    // Serial.println("RTC lost power, let's set the time!");
    // // When time needs to be set on a new device, or after a power loss, the
    // // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // // This line sets the RTC with an explicit date & time, for example to set
    // // January 21, 2014 at 3am you would call:
    // // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    // }

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