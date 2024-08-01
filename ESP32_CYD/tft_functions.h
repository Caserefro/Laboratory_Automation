
#ifndef TFT_FUNCTIONS_H
#define TFT_FUNCTIONS_H


void IRAM_ATTR timer_isr() {  //mutex not needed
  timeinfo.tm_min++;
  if (timeinfo.tm_min >= 60) {
    timeinfo.tm_min = 0;
    timeinfo.tm_hour++;
    if (timeinfo.tm_hour >= 24) {
      timeinfo.tm_hour = 0;
      timeinfo.tm_mday++;
      DayFlag = true;
      // Determine maximum day of the month based on the current month and year
      int days_in_month = 31;  // Set a default maximum (adjust for different months)
      switch (timeinfo.tm_mon) {
        case 3:
        case 5:
        case 8:
        case 10:  // April, June, September, November
          days_in_month = 30;
          break;
        case 1:  // February (check for leap year)
          days_in_month = (timeinfo.tm_year % 4 == 0 && (timeinfo.tm_year % 100 != 0 || timeinfo.tm_year % 400 == 0)) ? 29 : 28;
          break;
      }
      if (timeinfo.tm_mday > days_in_month) {
        timeinfo.tm_mday = 1;
        timeinfo.tm_mon++;  // Increment month if day wraps around
        if (timeinfo.tm_mon >= 12) {
          timeinfo.tm_mon = 0;
          timeinfo.tm_year++;  // Increment year if month wraps around
        }
      }
    }
  }
  vTaskResume(Timer_Task);  // Resume the TimerTask
}

void WriteText_tft(String text, int x, int y) {
  tft.setCursor(x, y);
  tft.println(text);
}

void Write_HomeScr_time() {
  String Time_24_Hour = "";  //Global?
  String Date = "";
  String aux = Time_24_Hour + "   " + Date;
  aux = "12:54";
  WriteText_tft(aux, 134, 7);  //fixed positions inside the screen.
}

void Write_HomeScr_date() {
  String Time_24_Hour = "";  //Global?
  String Date = "";
  String aux = Time_24_Hour + "   " + Date;
  aux = "01/07/24";
  WriteText_tft(aux, 226, 7);  //fixed positions inside the screen.
}

void Home_ScreenUpdate() {
  tft.pushImage(0, 0, 480, 320, Home_Screen);
  Write_HomeScr_time();
  Write_HomeScr_date();
  tft.pushImage(40, 1, 24, 28, SDState ? SD_ok : No_SD);
  tft.pushImage(77, 1, 35, 29, WiFiState ? WIFi_ok : No_WiFi);
  tft.pushImage(373, 1, 30, 30, Station1State ? Signal_ok_Station : No_Signal_Station);
  tft.pushImage(411, 1, 30, 30, Station2State ? Signal_ok_Station : No_Signal_Station);
  ScreenState = HomeScreenID;
}

void Borrowed_ScreenUpdate() {  // wip.
  tft.pushImage(0, 0, 480, 320, Borrowed);
  ScreenState = BorrowedScreenID;
}

void Ac_ScreenUpdate() {
  tft.pushImage(0, 0, 480, 320, Ac_Screen);
  ScreenState = AcScreenID;
}

void Weather_Screen() {  // values come from global
  tft.pushImage(0, 0, 480, 320, Weather_Ui);
  String WindSpd = "60 km/h";
  String WindDir = "NE";
  String UVidx = "10.1";
  String Temp = "35 C";
  String RainGauge = "1.2mm";
  String Hum = "60%";
  WriteText_tft(WindSpd, 115, 62);
  WriteText_tft(WindDir, 302, 62);
  WriteText_tft(UVidx, 115, 156);
  WriteText_tft(Temp, 299, 156);
  WriteText_tft(RainGauge, 115, 255);
  WriteText_tft(Hum, 299, 255);
  ScreenState = WeatherScreenID;
}

bool SD_Card_Available() {
  return 0;
}
//TouchData.x, TouchData.y, TouchData.size, Button_Layout[i].button[j])

//Station 1 and 2 dont have methods for checking availability. (might be changed.)


// button functions
bool isTouchWithinButton(int x, int y, int size, Button button) {
  return (x > button.x - (size / 2) && x < button.x + button.dimx + (size / 2) && y > button.y - (size / 2) && y < button.y + button.dimy + (size / 2));
}

void home_btn() {

  Home_ScreenUpdate();
}

// Home Screen button functions
void LockUnlock_btn() {
  //Send Command to master, so he sends it to espLock
}

void AcControllerScr_btn() {
  Ac_ScreenUpdate();
}

void WeatherScr_btn() {
  Weather_Screen();
}

void BorrowedScr_btn() {
  Borrowed_ScreenUpdate();
}

void ClubDesc_btn() {
  //print brief of club, data of creator
}

// Borrowed Screen button functions

void BorrowedUp_btn() {
}

void BorrowedDown_btn() {
}

void LeftArrow_btn() {
}

void RightArrow_btn() {
}

void Save_btn() {
}

// Ac Screen button functions

void OnOff_btn() {
}

void SwingFan_btn() {
}

void UpTemp_btn() {
}

void DownTemp_btn() {
}

#endif
