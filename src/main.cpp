#include "Arduino.h"
#include "CommandStation.h"
#ifdef WIFI
  #include "WiFi.h"

  WiFiClient DCCEX_STREAM;
#endif

void setup() {
  LOG_STREAM.begin(115200);
#ifdef WIFI
  LOG_STREAM.printf("Connecting to WiFi..\n");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(1000);
  LOG_STREAM.printf("Connected with IP: %s\n", WiFi.localIP());

  LOG_STREAM.println("Connecting to the server...\n");
  if (!DCCEX_STREAM.connect(DCCEX_ADDRESS, DCCEX_PORT)) {
    LOG_STREAM.println("Connection failed (system halted)");
    while (1) delay(1000);
  }
  LOG_STREAM.println("Connected to the server");
#else
  DCCEX_STREAM.begin(115200);
#endif

  LocoNetInterface.init();
  DCCEXInterface.init(&DCCEX_STREAM, &LOG_STREAM);
  XpressNetInterface.setup(Loco128, XNETPOPRT, XNETCONTROL);
  CommandStation.init();
}

void loop() {
  DCCEXInterface.check();
  XpressNetInterface.update();
  CommandStation.check();
}