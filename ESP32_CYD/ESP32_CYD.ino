#include "C:\Users\caser\Documents\GitHub\Laboratory_Automation\ESP32_CYD\ESP32_CYD.h"

void setup() {
  Serial.begin(115200);
  Serial.println("Start of program ---------------------------------------");
  vTaskDelay(100 / portTICK_PERIOD_MS);
  bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);

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
  xMutex = xSemaphoreCreateMutex();


  // Start task to handle command line interface events. Let's set it at a
  // higher priority but only run it once every 20 ms.
  xTaskCreate(TouchSensorTask,
              "Touch Sensor Task",
              2048,
              NULL,
              1,
              &TouchSensor_Task);

  // Start task to calculate average. Save handle for use with notifications.
  xTaskCreate(GraphicManagerTask,  //manager of all the screens Changes in
              "Publish data",
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
              "Touch Sensor Task",
              1024,
              NULL,
              1,
              &Timer_Task);
  // Delete "setup and loop" task
  Serial.println("end of setup succesfull --------------------------");
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
      if (xSemaphoreTake(xMutex, 0) == pdTRUE) {
        Serial.println("TouchSensorTask goes through mutex ****************************");
        for (int i = 0; i < ti.count; i++) {  // Loop through all touch points
          TouchData.x = x_coord;
          TouchData.y = map(y_coord, 0, 320, 320, 0);
          TouchData.size = ti.area[i];
          // Send the touch data to the queue
          xQueueSend(GT911_queue, (void *)&TouchData, 0);
        }
        xSemaphoreGive(xMutex);
      }
    }
    // Add a delay to prevent this task from consuming 100% CPU
    //  Serial.println("end of TouchSensorTask succesfull --------------------------");
    vTaskDelay(pdMS_TO_TICKS(35));  // Adjust the delay as needed (35)
  }
}

void GraphicManagerTask(void *parameters) {
  // TO DO:add features to prevent burn in. https://www.topwaydisplay.com/index.php/en/blog/tft-lcd-burn-in
  // TO DO:if ends up being annoying, make touch be ignored after scr changes
  // TO DO:Make draws for ClubDesc_btn, a general use loading screen, and Scrolling text for borrowed, in case server is not available.
  TouchDatadef xTouchData;
  while (1) {
    //  if (xSemaphoreTake(xMutex, 0) == pdTRUE) {
    if (xQueueReceive(GT911_queue, (void *)&xTouchData, 0) == pdTRUE) {  //used for touch and changing screens
      Serial.println("beggining of GraphicManagerTask succesfull -------------------------- DATA:");
      Serial.println(xTouchData.x);
      Serial.println(xTouchData.y);
      Serial.println(xTouchData.size);
      digitalWrite(TURNOFFSCREENPIN, HIGH);
      SleepingTimer = 1;
      bool buttonPressed = false;  // Flag to indicate button press and exit
      for (int i = 0; i < Total_Layouts && !buttonPressed; i++) {
        if (Button_Layout[i].ID == ScreenState) {  // selects current screen
          for (int j = 0; j < Button_Layout[i].numButtons; j++) {
            if (isTouchWithinButton(xTouchData.x, xTouchData.y, xTouchData.size, Button_Layout[i].button[j])) {  // executes function for each button
              Button_Layout[i].button[j].handler();                                                              // Call the button handler
              xQueueReset(GT911_queue);                                                                          // Clear the queue after processing
              buttonPressed = true;                                                                              // Set the flag to exit outer loop
              break;                                                                                             // Exit inner loop
            }
          }
        }
      }
    }
    /*else {
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  //used for updating state of hardware.
    }*/
    //  xSemaphoreGive(xMutex);
    // }
  }
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
      digitalWrite(TURNOFFSCREENPIN, LOW);
      SleepingTimer = 1;
    }
    if (!(ScreenState == HomeScreenID)) {  //check for homepage
      vTaskSuspend(NULL);
    }
    Write_HomeScr_time();  //update time in Scr
    if (DayFlag) {         //update date
      Write_HomeScr_date();
      DayFlag = false;
    }
    vTaskSuspend(NULL);
  }
}
