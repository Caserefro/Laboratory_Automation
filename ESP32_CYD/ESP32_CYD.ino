#include "C:\Users\caser\Documents\GitHub\Laboratory_Automation\ESP32_CYD\ESP32_CYD.h"


void setup() {
  Serial.begin(115200);
  Serial.println("Start of program ---------------------------------------");
  vTaskDelay(100 / portTICK_PERIOD_MS);
  bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
  DummydataSetup();
  vTaskDelay(100 / portTICK_PERIOD_MS);
  tft.init();
  tft.setRotation(1);
  tft.setSwapBytes(true);
  // Wrap test at right and bottom of screen
  tft.setTextWrap(true, true);
  // Font and background colour, background colour is used for anti-alias blending
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  // Load the font
  tft.loadFont(newrodin);
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  Home_ScreenUpdate();

  timer = timerBegin(timer_id, prescaler, true);
  timerAttachInterrupt(timer, &timer_isr, true);
  timerAlarmWrite(timer, threshold, true);
  timerAlarmEnable(timer);

  // Create message queue before it is used
  GT911_queue = xQueueCreate(GT911_QUEUE_LEN, sizeof(TouchData));

  xTaskCreate(TouchSensorTask,
              "Touch Sensor Task",
              2048,
              NULL,
              1,
              &TouchSensor_Task);
  xTaskCreate(GraphicManagerTask,  //manager of all the screens Changes in
              "Graphic Manager Task",
              16384,
              NULL,
              1,
              &GraphicManager_Task);
  /*
  xTaskCreate(SyncingTask,
              "Publish data",
              2048,
              NULL,
              1,
              &Syncing_Task);*/

  xTaskCreate(TimerTask,
              "Timer Task",
              1024,
              NULL,
              1,
              &Timer_Task);
  // Delete "setup and loop" task
  Serial.println("end of setup succesfull --------------------------");
  xTouchScrSyncSemaphore = xSemaphoreCreateBinary();
  xScrWriteSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(xTouchScrSyncSemaphore);  // Initially available
  xSemaphoreGive(xScrWriteSemaphore);      // Initially available
  vTaskDelete(NULL);
}
void loop() {
  //this will get deleted
}

void printFreeStackSpace(const char *taskName) {
  UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
  Serial.print(taskName);
  Serial.print(" - Stack High Water Mark: ");
  Serial.println(stackHighWaterMark);
}

void TouchSensorTask(void *parameters) {
  while (1) {
    if (bbct.getSamples(&ti)) {
      for (int i = 0; i < ti.count; i++) {  // Loop through all touch points
        TouchData.x = x_coord;
        TouchData.y = map(y_coord, 0, 320, 320, 0);
        TouchData.size = ti.area[i];

        if (xSemaphoreTake(xTouchScrSyncSemaphore, 0) == pdTRUE) {  // Check if semaphore is available
          SleepingTimer = SLEEPTIMER;
          xQueueSend(GT911_queue, (void *)&TouchData, 0);
          xTaskNotify(GraphicManager_Task, 0, eIncrement);
          vTaskDelay(pdMS_TO_TICKS(100));
          xSemaphoreGive(xTouchScrSyncSemaphore);  // Release semaphore
        }
      }
    }
  }
}


void GraphicManagerTask(void *parameters) {
  TouchDatadef xTouchData;
  while (ulTaskNotifyTake(pdFALSE, portMAX_DELAY)) {
    do {
      switch (AnimationState) {
        case ScreenSaverEnabler:
          if (ScreenState != ScreenSaverID) {
            digitalWrite(TURNOFFSCREENPIN, LOW);
            ScreenSaverX = 0;
            LastScreenState = ScreenState;
            ScreenState = ScreenSaverID;
          }
          if (xSemaphoreTake(xScrWriteSemaphore, portMAX_DELAY) == pdTRUE) {  // Check if semaphore is available
            tft.fillScreen(TFT_BLACK);
            tft.fillRect(ScreenSaverX, 0, 60, 320, TFT_WHITE);
            xSemaphoreGive(xScrWriteSemaphore);  // Release semaphore
          }
          ScreenSaverX += 60;
          if (ScreenSaverX == 480) ScreenSaverX = 0;
          vTaskDelay(pdMS_TO_TICKS(200));
          break;
        case ScrollingTextBorrowedEnabler:
          if (millis() - ScrolingTextAUXCOUNTER > ScrollingTextDelay || ScrollingTextDelay == 3001) {
            if (ScrollingTextDelay == 3001) {
              memset(&ItemTop, 0, sizeof(ItemTop));
              memset(&NameandNCTop, 0, sizeof(NameandNCTop));
              memset(&DatesTop, 0, sizeof(DatesTop));
              memset(&ItemBottom, 0, sizeof(ItemBottom));
              memset(&NameandNCBottom, 0, sizeof(NameandNCBottom));
              memset(&DatesBottom, 0, sizeof(DatesBottom));
            }
            PrintScrollingText(ItemTopSprite, borrowedItems[((CurrentPair - 1) * 2)].Item, ItemTop, 37, 20, 297, 20);  //position / size
            ScrollingTextBuffer = String(borrowedItems[((CurrentPair - 1) * 2)].Name + " " + borrowedItems[((CurrentPair - 1) * 2)].NCID);
            PrintScrollingText(NameandNCTopSprite, ScrollingTextBuffer, NameandNCTop, 37, 45, 297, 20);
            ScrollingTextBuffer = String("Borrowed: " + borrowedItems[((CurrentPair - 1) * 2)].Time + " " + borrowedItems[((CurrentPair - 1) * 2)].Date + " Returned: " + borrowedItems[((CurrentPair - 1) * 2)].TimeReturned + " " + borrowedItems[((CurrentPair - 1) * 2)].DateReturned);
            PrintScrollingText(DatesTopSprite, ScrollingTextBuffer, DatesTop, 37, 70, 297, 20);
            if (borrowedItems[((CurrentPair - 1) * 2) + 1].ItemID != 0) {  // here current pair is never out of limits, so this one is the only optional one.
              PrintScrollingText(ItemBottomSprite, borrowedItems[((CurrentPair - 1) * 2) + 1].Item, ItemBottom, 37, 217, 297, 20);
              ScrollingTextBuffer = String(borrowedItems[((CurrentPair - 1) * 2) + 1].Name + " " + borrowedItems[((CurrentPair - 1) * 2) + 1].NCID);
              PrintScrollingText(NameandNCBottomSprite, ScrollingTextBuffer, NameandNCBottom, 37, 242, 297, 20);
              ScrollingTextBuffer = String("Borrowed: " + borrowedItems[((CurrentPair - 1) * 2) + 1].Time + " " + borrowedItems[((CurrentPair - 1) * 2) + 1].Date + " Returned: " + borrowedItems[((CurrentPair - 1) * 2) + 1].TimeReturned + " " + borrowedItems[((CurrentPair - 1) * 2) + 1].DateReturned);
              PrintScrollingText(DatesBottomSprite, ScrollingTextBuffer, DatesBottom, 37, 267, 297, 20);
            }

            ScrolingTextAUXCOUNTER = millis();
            if (ScrollingTextDelay == 3000) {
              ScrollingTextDelay = 30;
            }
            if (ScrollingTextDelay == 3001) {
              ScrollingTextDelay = 3000;
            }
          } else {
            vTaskDelay(pdMS_TO_TICKS(15));
          }
          break;
      }
      if (xQueueReceive(GT911_queue, (void *)&xTouchData, 0) == pdTRUE) {
        if (xSemaphoreTake(xTouchScrSyncSemaphore, portMAX_DELAY) == pdTRUE) {  // Ensure screen is not updating
          if (xSemaphoreTake(xScrWriteSemaphore, portMAX_DELAY) == pdTRUE) {    // Check if semaphore is available
            Serial.println("DEBUG: A touch was registered!: -------------------------- DATA:");
            Serial.println(xTouchData.x);
            Serial.println(xTouchData.y);
            Serial.println(xTouchData.size);

            if (AnimationState == ScreenSaverEnabler) {  // This might get moved out
              AnimationState = 0;
              digitalWrite(TURNOFFSCREENPIN, HIGH);
              RecoverScreenState();
              vTaskDelay(pdMS_TO_TICKS(300));
            } else {
              bool buttonPressed = false;
              for (int i = 0; i < Total_Layouts && !buttonPressed; i++) {
                if (Button_Layout[i].ID == ScreenState) {
                  for (int j = 0; j < Button_Layout[i].numButtons; j++) {
                    if (isTouchWithinButton(xTouchData.x, xTouchData.y, xTouchData.size, Button_Layout[i].button[j])) {
                      Button_Layout[i].button[j].handler();
                      buttonPressed = true;
                      xQueueReset(GT911_queue);
                      break;
                    }
                  }
                }
              }
            }
            xSemaphoreGive(xScrWriteSemaphore);  // Release semaphore
          }
          xSemaphoreGive(xTouchScrSyncSemaphore);  // Release semaphore to allow touch processing
        }
      }
    } while (AnimationState != 0);
  }
  vTaskSuspend(NULL);
}


/*
void SyncingTask(void *parameters) {
  while (1) {
    if (xQueueReceive(GT911_queue, (void *)&Sensordata, 7) == pdTRUE) {  
      wspackage += (String(Sensordata.zGvalue) + "," + String(Sensordata.Timestamp) + ",");
      if (wspackage.length() > 300) {TimerTask
        ws.textAll(wspackage);
        Serial.println(wspackage);
        wspackage = "";
      }
    }
  }
}
*/
void TimerTask(void *parameters) {
  while (1) {
    Serial.println("TimerTask ran succesfull --------------------------");
    SleepingTimer--;
    if (SleepingTimer == 0) {
      AnimationState = ScreenSaverEnabler;
      //   xTaskNotifyGive(ScreenSaver_Task);
      xTaskNotify(GraphicManager_Task, 0, eIncrement);  // Notify Task C to start
    } else {
      if (ScreenState == HomeScreenID) {  //check for homepage
        Write_HomeScr_time();             //update time in Scr
        if (DayFlag) {                    //update date
          Write_HomeScr_date();
          DayFlag = false;
        }
      }
    }
    vTaskSuspend(NULL);
  }
}
