#include "Arduino.h"
#include "CommandStation.h"
//#include "LocoNetInterface.h"
//#include "DCCEXInterface.h"
//#include "XpressNetInterface.h"
//#include "Throttle.h"

void setup() {
  DCCEX_LOG.begin(115200);
  DCCEX_STREAM.begin(115200);

  LocoNetInterface.init();
  DCCEXInterface.init(&DCCEX_STREAM, &DCCEX_LOG);
  XpressNetInterface.setup(Loco128, XNETPOPRT, XNETCONTROL);
  CommandStation.init();
  
  DIAG("%s\n", CS_NAME);
}

void loop() {
  DCCEXInterface.check();
  XpressNetInterface.update();
  CommandStation.check();
}