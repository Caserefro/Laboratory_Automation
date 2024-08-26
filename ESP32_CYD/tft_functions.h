
#ifndef TFT_FUNCTIONS_H
#define TFT_FUNCTIONS_H

void DummydataSetup() {
  borrowedItems[0].ItemID = 1;
  borrowedItems[0].Item = "Cautin, Proyector";
  borrowedItems[0].Name = "Francisco Javier Cisneros Medina";
  borrowedItems[0].NCID = 22020599;
  borrowedItems[0].Time = "12:40";
  borrowedItems[0].Date = "15/07/2024";
  borrowedItems[0].Returned = true;
  borrowedItems[0].TimeReturned = "13:00";
  borrowedItems[0].DateReturned = "15/07/2024";

  borrowedItems[1].ItemID = 2;
  borrowedItems[1].Item = "Cable HDMI, Resistores, Transformador";
  borrowedItems[1].Name = "Carlos Parra Andrade";
  borrowedItems[1].NCID = 22020599;
  borrowedItems[1].Time = "15:30";
  borrowedItems[1].Date = "16/07/2024";
  borrowedItems[1].Returned = false;
  borrowedItems[1].TimeReturned = "--:--";
  borrowedItems[1].DateReturned = "--/--/----";

  borrowedItems[2].ItemID = 3;
  borrowedItems[2].Item = "Arduino UNO, Protoboard, Libro Dennis Zill";
  borrowedItems[2].Name = "Walter White";
  borrowedItems[2].NCID = 22020556;
  borrowedItems[2].Time = "09:00";
  borrowedItems[2].Date = "17/07/2024";
  borrowedItems[2].Returned = true;
  borrowedItems[2].TimeReturned = "10:00";
  borrowedItems[2].DateReturned = "18/07/2024";

  borrowedItems[3].ItemID = 4;
  borrowedItems[3].Item = "Libro Luna de pluton ";
  borrowedItems[3].Name = "Average Joe";
  borrowedItems[3].NCID = 2203478;
  borrowedItems[3].Time = "11:15";
  borrowedItems[3].Date = "18/07/2024";
  borrowedItems[3].Returned = true;
  borrowedItems[3].TimeReturned = "";
  borrowedItems[3].DateReturned = "";

  borrowedItems[4].ItemID = 5;
  borrowedItems[4].Item = "Camera";
  borrowedItems[4].Name = "Charlie Green";
  borrowedItems[4].NCID = 90;
  borrowedItems[4].Time = "14:30";
  borrowedItems[4].Date = "19/07/2024";
  borrowedItems[4].Returned = false;
  borrowedItems[4].TimeReturned = "--:--";
  borrowedItems[4].DateReturned = "--/--/----";
}

void printFirstFiveItems() {
  for (int i = 0; i < 5; i++) {
    Serial.print("ItemID: ");
    Serial.println(borrowedItems[i].ItemID);

    Serial.print("Item: ");
    Serial.println(borrowedItems[i].Item);

    Serial.print("Name: ");
    Serial.println(borrowedItems[i].Name);

    Serial.print("NCID: ");
    Serial.println(borrowedItems[i].NCID);

    Serial.print("Time: ");
    Serial.println(borrowedItems[i].Time);

    Serial.print("Date: ");
    Serial.println(borrowedItems[i].Date);

    Serial.print("Returned: ");
    Serial.println(borrowedItems[i].Returned ? "true" : "false");

    Serial.print("TimeReturned: ");
    Serial.println(borrowedItems[i].TimeReturned);

    Serial.print("DateReturned: ");
    Serial.println(borrowedItems[i].DateReturned);

    Serial.println("------------");
  }
}

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

void PrintScrollingText(TFT_eSprite& ScrollingText, String& text, ScrollingTextSprite& ScrollText, int x, int y, int xsize, int ysize) {
  // Check initialization
  if (!ScrollText.initCall) {
    ScrollingText.createSprite(xsize, ysize);  // Create sprite
    ScrollingText.fillSprite(TFT_WHITE);
    ScrollingText.setTextColor(TFT_BLACK);
    ScrollingText.loadFont(newrodin);
    ScrollingText.setTextWrap(false, false);
    ScrollText.initCall = true;  // Mark as initialized
  }

  // Check if text fits in the display
  if (xsize - 20 - ScrollingText.textWidth(text) > 0 && !ScrollText.printed) {
    ScrollingText.setCursor(0, 0);
    ScrollingText.println(text);
    ScrollingText.pushSprite(x, y);
    ScrollText.printed = true;  // Mark as printed
    return;
  }
  if (xsize - 20 - ScrollingText.textWidth(text) > 0 && ScrollText.printed)
    return;
  // Manage scrolling
  while (!ScrollText.printed) {
    ScrollText.Buffer = text.substring(ScrollText.startIndex, ScrollText.endIndex + 2);
    if (ScrollingText.textWidth(ScrollText.Buffer) > xsize - 20) {
      ScrollText.Buffer = text.substring(ScrollText.startIndex, ScrollText.endIndex + 1);
      ScrollingText.setCursor(0, 0);
      ScrollingText.println(ScrollText.Buffer);
      ScrollingText.pushSprite(x, y);
      ScrollText.startIndex = ScrollText.endIndex;
      ScrollText.printed = true;

      ScrollText.maxpxposition = ScrollingText.textWidth(ScrollText.Buffer) - ScrollingText.textWidth(String(text[ScrollText.startIndex + 1]));
      ScrollText.pxcount = 1;

      Serial.println(text.substring(ScrollText.startIndex));
      Serial.println(text.length() - ScrollText.startIndex);
      Serial.println(ScrollText.maxpxposition);
      ScrollText.Buffer = "";
      return;
    } else {
      ScrollText.endIndex++;
    }
  }
  //start Scrolling
  if (!ScrollText.SecondCall) {
    ScrollingText.setScrollRect(0, 0, xsize, ysize, TFT_WHITE);
    ScrollText.SecondCall = true;
  }
  // Handle scrolling logic
  ScrollText.pxcount--;
  if (ScrollText.pxcount == 0) {
    if (ScrollText.startIndex == text.length()) {
      ScrollText.startIndex = 0;
      ScrollText.pxcount = 16;
    } else {
      if (ScrollText.startIndex == 0) {
        ScrollingText.setCursor(ScrollText.maxpxposition, 0);
        ScrollingText.println(text[0]);
        ScrollText.Buffer = text.substring(ScrollText.startIndex, ScrollText.startIndex + 2);
        //Serial.println(ScrollText.Buffer);
        int pxbeforenext = ScrollingText.textWidth(ScrollText.Buffer) - ScrollingText.textWidth(String(ScrollText.Buffer[1]));
        // Serial.println(pxbeforenext);
        ScrollText.pxcount = pxbeforenext;
        ScrollText.startIndex++;
      } else {
        ScrollingText.setCursor(ScrollText.maxpxposition, 0);
        ScrollingText.println(ScrollText.Buffer[1]);
        ScrollText.Buffer = text.substring(ScrollText.startIndex, ScrollText.startIndex + 2);
        // Serial.println(ScrollText.Buffer);
        int pxbeforenext = ScrollingText.textWidth(ScrollText.Buffer) - ScrollingText.textWidth(String(ScrollText.Buffer[1]));
        // Serial.println(pxbeforenext);
        ScrollText.pxcount = pxbeforenext;
        ScrollText.startIndex++;
      }
    }
  }
  ScrollingText.pushSprite(x, y);
  ScrollingText.scroll(-1);  // Scroll the sprite
}


void Write_HomeScr_time() {
  char timeBuffer[6];  // Buffer for the time format "MM:HH"
  // Format the time as "min:hour"
  strftime(timeBuffer, sizeof(timeBuffer), "%H:%M", &timeinfo);
  String TimeReturned = String(timeBuffer);
  WriteText_tft(TimeReturned, 134, 7);  //fixed positions inside the screen.
}

void Write_HomeScr_date() {
  char dateBuffer[11];  // Buffer for the date format "DD/MM/YY"
  // Format the date as "Day/Month/last two digits of year"
  strftime(dateBuffer, sizeof(dateBuffer), "%d/%m/%y", &timeinfo);
  String DateReturned = String(dateBuffer);
  WriteText_tft(DateReturned, 226, 7);  //fixed positions inside the screen.
}

void Home_ScreenUpdate() {
  tft.pushImage(0, 0, 480, 320, Home_Screen);
  tft.fillRect(130, 5, 230, 25, TFT_WHITE);
  Write_HomeScr_time();
  Write_HomeScr_date();
  tft.pushImage(40, 1, 24, 28, SDState ? SD_ok : No_SD);
  tft.pushImage(77, 1, 35, 29, WiFiState ? WIFi_ok : No_WiFi);
  tft.pushImage(373, 1, 30, 30, Station1State ? Signal_ok_Station : No_Signal_Station);
  tft.pushImage(411, 1, 30, 30, Station2State ? Signal_ok_Station : No_Signal_Station);
  ScreenState = HomeScreenID;
  AnimationState = 0;  //Disables all current animations.
}


void BorrowedScreen() {
  if (ScreenState != BorrowedScreenID) {
    tft.pushImage(0, 0, 480, 320, Borrowed);
    ScreenState = BorrowedScreenID;
  }
  // Check if CurrentPair is out of limits
  if (borrowedItems[((CurrentPair - 1) * 2)].ItemID == 0) {
    CurrentPair--;
    return;
  }
  // Clear screen and display updated screen contents
  tft.pushImage(0, 0, 480, 320, Borrowed);
  tft.pushImage(21, 9, 438, 104, TopReturnedBox);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.fillRect(218, 148, 50, 25, TFT_BLACK);
  WriteText_tft(String(CurrentPair), 222, 150);  //fixed positions inside the screen.
  tft.setTextColor(TFT_BLACK, TFT_WHITE);

  // Display top item status
  if (borrowedItems[((CurrentPair - 1) * 2)].Returned == 1) {
    tft.pushImage(349, 15, 103, 98, ReturnedUpbtn);
  } else {
    tft.pushImage(349, 15, 103, 98, NotReturnedUP);
  }

  // Check for the presence of the bottom item and display accordingly
  if (borrowedItems[((CurrentPair - 1) * 2) + 1].ItemID != 0) {
    tft.pushImage(21, 207, 438, 104, BottomReturnedBox);
    if (borrowedItems[((CurrentPair - 1) * 2) + 1].Returned == 1) {
      tft.pushImage(349, 207, 103, 98, ReturnedDownBtn);
    } else {
      tft.pushImage(349, 207, 103, 98, NotReturnedDown);
    }
  } else {
    // Draw padding or additional UI elements if needed
    tft.pushImage(21, 204, 18, 3, Padding);
  }
  AnimationState = ScrollingTextBorrowedEnabler;
  ScrollingTextDelay = 3001;
  ScrolingTextAUXCOUNTER = 0;
}

void Ac_ScreenUpdate() {
  tft.pushImage(0, 0, 480, 320, Ac_Screen);
  ScreenState = AcScreenID;
}


void Weather_Screen() {  // values come from global
  LoadingPopup();
  OP_WEATHER_Wrapper();
  tft.pushImage(0, 0, 480, 320, Weather_Ui);
  String WindSpd = String((int)WeatherData.WindSpd) + " km/h";
  String Temp = String((int)WeatherData.Temp) + " C";
  String RainGauge = String((int)WeatherData.RainG) + " mm";
  String Hum = String((int)WeatherData.RelHum) + "%";
  WriteText_tft(WindSpd, 115, 62);
  WriteText_tft(WeatherData.WindDir, 302, 62);
  WriteText_tft(String((int)WeatherData.UVidx), 115, 156);
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
  return (x > button.x - (size / 3) && x < button.x + button.dimx + (size / 3) && y > button.y - (size / 3) && y < button.y + button.dimy + (size / 3));
}

// Home Screen button functions
void LockUnlock_btn() {
  //Send Command to master, so he sends it to espLock
  LoadingPopup();
  OP_OPEN_CLOSE_Wrapper();
  RecoverScreenState();
}

void BorrowedScr_btn() {
  LoadingPopup();
  CurrentPair = 1;
  UpdateBorrowedItems();
  BorrowedScreen();
}

void ClubDesc_btn() {
  tft.pushImage(26, 43, 430, 269, Info_PopUp);
  ScreenState = InfoPopupID;
}

// Borrowed Screen button functions

void BorrowedUp_btn() {
  LoadingPopup();
}

void BorrowedDown_btn() {
  LoadingPopup();
}

void LeftArrow_btn() {
  CurrentPair--;
  if (CurrentPair == 0) {
    CurrentPair = 1;
    return;
  }
  BorrowedScreen();
}

void RightArrow_btn() {
  CurrentPair++;
  BorrowedScreen();
}

void Save_btn() {  //Pending. Will dump all borrowed data into the sd card.
  LoadingPopup();
}

// Ac Screen button functions

void OnOff_btn() {
  LoadingPopup();
  OP_AC_Wrapper(OP_ACONOFF);
  Ac_ScreenUpdate();
}

void SwingFan_btn() {
  LoadingPopup();
  OP_AC_Wrapper(OP_ACSWINGFAN);
  Ac_ScreenUpdate();
}

void UpTemp_btn() {
  LoadingPopup();
  OP_AC_Wrapper(OP_ACUPTEMP);
  Ac_ScreenUpdate();
}

void DownTemp_btn() {
  LoadingPopup();
  OP_AC_Wrapper(OP_ACDOWNTEMP);
  Ac_ScreenUpdate();
}

void LoadingPopup() {
  tft.fillCircle(240, 160, 85, TFT_BLACK);
  tft.pushImage(192, 104, 96, 112, Hourglass);
}

void RecoverScreenState() {
  switch (LastScreenState) {
    case HomeScreenID:
      Home_ScreenUpdate();
      break;
    case InfoPopupID:
      Home_ScreenUpdate();
      ClubDesc_btn();
      break;
    case BorrowedScreenID:
      BorrowedScreen();
      break;
    case AcScreenID:
      Ac_ScreenUpdate();
      break;
    case WeatherScreenID:
      Weather_Screen();
      break;
  }
}

#endif
