#define CS_NAME "EverFocus"
#define LCD_ROWS        2
#define LCD_COLS        20
#define ENCODER_CLK_PIN 15
#define ENCODER_DT_PIN  4
#define ENCODER_SW1_PIN 32
#define ENCODER_SW2_PIN 33
#define ENCODER_SW3_PIN 27
#define ENCODER_SWD_PIN 35
#define MAX_SLOT_NUM    127
#define LOG_STREAM      Serial
//#define DEBUG_EVERFOCUS

//Wifi Interface 
#define WIFI 1
#define WIFI_SSID "Vodafone-D6D4"          // WiFi SSID name here
#define WIFI_PASS "QWswF2vuz85dyacr"    // WiFi password here

//DCCEX Interface
#define DCCEX 1  
#define DCCEX_ADDRESS { 192, 168, 0, 45 }   // IP address of your EX-CommandStation             
#define DCCEX_PORT 2560                     // Port number of your EX-CommandStation             
#define DCCEX_STREAM Serial2  

//LocoNet Interface
#define LOCONET 1               
#ifdef ESP32
  #define LOCONET_RX 5
  #define LOCONET_TX 18
#endif
#ifdef __AVR__
  #define LOCONET_TX 7
#endif

//XpressNet Interface
#define XPRESSNET 1   
#define XNETPOPRT    19       //Max485 Busdriver RX/TX-PIN   
#define XNETCONTROL  23       //Max485 Busdriver Send/Receive-PIN

#define TARGET_SPEEDS     {0,    5,   10,  15,  40,  60,  90,  110,  127}  //speed in steps
#define ACCEL_DELAY_TIMES {1000, 600, 550, 500, 450, 400, 350, 300, 250};   //ms
#define BRAKE_DELAY_TIMES {1000, 500, 200, 125, 50, 25, 1};   //ms
#define SPEED_AMOUNT 2

//Button codes
#define BTN_REC      0
#define BTN_STOP2    1
#define BTN_DRV      2
#define BTN_AUTO     3
#define BTN_F1       4
#define BTN_HOLD     5
#define BTN_POSITION 6
#define BTN_DISPLAY  7
#define BTN_PLAY     8
#define BTN_HOME     9
#define BTN_CAM      10
#define BTN_FOCUSF   11
#define BTN_F3       12
#define BTN_FOCUSN   13
#define BTN_APAN     14
#define BTN_MODE     15
#define BTN_COPY     16
#define BTN_CLR      17
#define BTN_4        18
#define BTN_SET      19
#define BTN_1        21
#define BTN_7        22
#define BTN_SEQ      23
#define BTN_MENU     24
#define BTN_ENTER    25
#define BTN_6        26
#define BTN_3        29
#define BTN_9        30
#define BTN_ESC      31
#define BTN_CALL     40
#define BTN_0        41
#define BTN_5        42
#define BTN_SHIFT    43
#define BTN_2        45
#define BTN_8        46
#define BTN_SEARCH   47
#define BTN_PAUSE    48
#define BTN_ZOOMIN   51
#define BTN_F4       52
#define BTN_ZOOMOUT  53
#define BTN_ZOOM     55
#define BTN_STOP     56
#define BTN_MON      58
#define BTN_IRIS1    59
#define BTN_F2       60
#define BTN_IRIS2    61
#define BTN_TOUR     62
#define BTN_SELECT   63
#define BTN_ERROR    65

//Joystick 
#define JOY_X_PIN           39
#define JOY_Y_PIN           36
#define JOY_X_CENTER        2048
#define JOY_Y_CENTER        2048
#define JOY_LEFT_THRESHOLD  1048
#define JOY_RIGHT_THRESHOLD 3048
#define JOY_UP_THRESHOLD    1048
#define JOY_DOWN_THRESHOLD  3048