#define CS_NAME "EverFocus CommandStation"
#define LCD_ROWS 2
#define LCD_COLS 16
#define ROTARY_A 15
#define ROTARY_B 4
#define ROTARY_S 2

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

