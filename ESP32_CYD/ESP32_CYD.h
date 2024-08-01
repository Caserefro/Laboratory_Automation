#ifndef ESP32_CYD_H
#define ESP32_CYD_H

#include <TFT_eSPI.h>
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
TFT_eSprite sprite = TFT_eSprite(&tft);

#define TOUCH_SDA 33
#define TOUCH_SCL 32
#define TOUCH_INT 21
#define TOUCH_RST 25
#include <bb_captouch.h>
BBCapTouch bbct;
TOUCHINFO ti;
#define x_coord ti.y[i]  //Declared due to form factor constraints .
#define y_coord ti.x[i]

struct TouchDatadef {
  int x;
  int y;
  int size;
} TouchData;

#define MAX_BUTTONS_PER_GROUP 8  //in case more buttons are created, beware this limit.
#define MAX_GROUPS 10

struct Button {
  int x;
  int y;
  int dimx;
  int dimy;
  void (*handler)(void);  // Function pointer for the button handler
};
struct Layout {
  unsigned char ID;
  int numButtons;  // Number of buttons in the layout
  Button button[MAX_BUTTONS_PER_GROUP];
};

static TaskHandle_t TouchSensor_Task = NULL;
static TaskHandle_t GraphicManager_Task = NULL;
static TaskHandle_t Syncing_Task = NULL;
static TaskHandle_t Timer_Task = NULL;

static QueueHandle_t GT911_queue;
enum { GT911_QUEUE_LEN = 10 };  // Number of slots in message queue
static SemaphoreHandle_t xMutex;

bool WiFiState = 0, SDState = 0, Station1State = 0, Station2State = 0;

struct tm timeinfo;
volatile bool DayFlag = 0, MinuteFlag = 0;

hw_timer_t* timer = NULL;
uint8_t timer_id = 0;
uint16_t prescaler = 80;             // Between 0 and 65 535
int threshold = (1000000 / 2) * 60;  // 64 bits value (limited to int size of 32bits)

struct BorrowedItem {
  int ItemID = 0;
  String Item = "";  // 18 usable + null (max amount that can be fit in a row)
  String Name = "";  // 15 usable + null
  int NCID = 0;      // 2 usable + null
  String Time = "";  // hour+date 1240+null = 12:40
  String Date = "";  // day+month+year+null
  bool Returned = 0;
  //internal
  String TimeReturned = "";  // hour+date 1240+null = 12:40
  String DateReturned = "";  // day+month+year+null
};

#define TURNOFFSCREENPIN 27

#define HomeScreenID 1
#define InfoPopupID 2
#define BorrowedScreenID 3
#define AcScreenID 4
#define WeatherScreenID 5

int Total_Layouts = 5;  // Update this count based on the number of initialized groups
unsigned char ScreenState = HomeScreenID;
int SleepingTimer = 2;

#include "C:\Users\caser\Documents\GitHub\Laboratory_Automation\ESP32_CYD\UI elements\Home_Screen.h"
#include "C:\Users\caser\Documents\GitHub\Laboratory_Automation\ESP32_CYD\UI elements\Ac_Screen.h"
#include "C:\Users\caser\Documents\GitHub\Laboratory_Automation\ESP32_CYD\UI elements\Borrowed_Screen.h"
#include "C:\Users\caser\Documents\GitHub\Laboratory_Automation\ESP32_CYD\UI elements\Icons.h"
#include "C:\Users\caser\Documents\GitHub\Laboratory_Automation\ESP32_CYD\UI elements\new_rodin.h"
#include "C:\Users\caser\Documents\GitHub\Laboratory_Automation\ESP32_CYD\UI elements\Weather_Screen.h"

#include "C:\Users\caser\Documents\GitHub\Laboratory_Automation\ESP32_CYD\Credentials.h"
#include "C:\Users\caser\Documents\GitHub\Laboratory_Automation\ESP32_CYD\tft_functions.h"
#include "C:\Users\caser\Documents\GitHub\Laboratory_Automation\ESP32_CYD\Button_Layout.h"
#include "C:\Users\caser\Documents\GitHub\Laboratory_Automation\ESP32_CYD\WiFi_functions.h"
#endif