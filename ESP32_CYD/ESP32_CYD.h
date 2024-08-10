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

struct ScrollingTextSprite {
  int complete = false;
  int startIndex = 0;
  int endIndex = 0;
  int maxpxposition = 0;
  int pxbeforenext = 0;
  int pxcount = 0;
  bool initCall = false;
  bool SecondCall = false;
  bool printed = 0;
  String Buffer = "";
};
int ScrollingTextDelay = 0;
long ScrolingTextAUXCOUNTER = 0;
String ScrollingTextBuffer;

TFT_eSprite ItemTopSprite = TFT_eSprite(&tft);
TFT_eSprite NameandNCTopSprite = TFT_eSprite(&tft);
TFT_eSprite DatesTopSprite = TFT_eSprite(&tft);
ScrollingTextSprite ItemTop;
ScrollingTextSprite NameandNCTop;
ScrollingTextSprite DatesTop;

TFT_eSprite ItemBottomSprite = TFT_eSprite(&tft);
TFT_eSprite NameandNCBottomSprite = TFT_eSprite(&tft);
TFT_eSprite DatesBottomSprite = TFT_eSprite(&tft);
ScrollingTextSprite ItemBottom;
ScrollingTextSprite NameandNCBottom;
ScrollingTextSprite DatesBottom;

#define TURNOFFSCREENPIN 27
#define SLEEPTIMER 2
int ScreenSaverX = 0;

//Screens
#define HomeScreenID 1
#define InfoPopupID 2
#define BorrowedScreenID 3
#define AcScreenID 4
#define WeatherScreenID 5
#define ScreenSaverID 6

//Animations in Screens
unsigned char AnimationState = 0;       // 0 = No animation required.
#define ScreenSaverEnabler 1            //Belongs to ScreenSaverID
#define ScrollingTextBorrowedEnabler 2  //Belongs to BorrowedScreenID

int Total_Layouts = 5;  // Update this count based on the number of initialized groups
unsigned char ScreenState = HomeScreenID;
unsigned char LastScreenState = HomeScreenID;
int SleepingTimer = 2;
int CurrentPair = 1;

//task and kernel objects ----------------------------------------------------------------------------------------------------
static TaskHandle_t TouchSensor_Task = NULL;
static TaskHandle_t GraphicManager_Task = NULL;
static TaskHandle_t Syncing_Task = NULL;
static TaskHandle_t Timer_Task = NULL;

static QueueHandle_t GT911_queue;
enum { GT911_QUEUE_LEN = 10 };  // Number of slots in message queue

SemaphoreHandle_t xTouchScrSyncSemaphore;  //Used for coordinating the Screen updating and the touch.
SemaphoreHandle_t xScrWriteSemaphore;      //Used for coordinating the Screen updating and the touch.

bool WiFiState = 0, SDState = 0, Station1State = 0, Station2State = 0;  //Flags for HomeScreen

struct tm timeinfo;
volatile bool DayFlag = 0, MinuteFlag = 0;
hw_timer_t* timer = NULL;
uint8_t timer_id = 0;
uint16_t prescaler = 80;             // Between 0 and 65 535
int threshold = (1000000 / 2) * 60;  // 64 bits value (limited to int size of 32bits)

#include "UI elements\Home_Screen.h"
#include "UI elements\Ac_Screen.h"
#include "UI elements\Borrowed_Screen.h"
#include "UI elements\Icons.h"
#include "UI elements\new_rodin.h"
#include "UI elements\Weather_Screen.h"

#include "Credentials.h"
#include "Client_Definitions.h"
#include "tft_functions.h"
#include "Button_Layout.h"
#include "http_functions.h"
#include "http_Wrappers.h"
#include "Step2Packages.h"
#endif