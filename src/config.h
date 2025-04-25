#define CS_NAME "EverFocus CommandStation"
#define LCD_ROWS 2
#define LCD_COLS 20
#define ROTARY_ENCODER_A_PIN 15
#define ROTARY_ENCODER_B_PIN 4
#define ROTARY_ENCODER_BUTTON_PIN 2
#define DEBUG_EVERFOCUS

//DCCEX Interface
#define DCCEX                  
#define DCCEX_LOG Serial
#define DCCEX_STREAM Serial2  

//LocoNet Interface
#define LOCONET                
#ifdef ESP32
  #define LOCONET_RX 5
  #define LOCONET_TX 18
#endif
#ifdef __AVR__
  #define LOCONET_TX 7
#endif

//XpressNet Interface
#define XPRESSNET   
#define XNETPOPRT   19       //Max485 Busdriver RX/TX-PIN   
#define XNETCONTROL 23       //Max485 Busdriver Send/Receive-PIN

#define TARGET_SPEEDS     {0,    5,   10,  15,  40,  60,  90,  110,  127}  //speed in steps
#define ACCEL_DELAY_TIMES {1000, 600, 550, 500, 450, 400, 350, 300, 250};   //ms
#define BRAKE_DELAY_TIMES {1000, 500, 200, 125, 50, 25, 1};   //ms
#define SPEED_AMOUNT 1


