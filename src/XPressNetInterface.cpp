#include "XpressNetInterface.h"


XpressNetInterfaceClass::XpressNetInterfaceClass() : XpressNetMasterClass() {
}

/* 
void XpressNetInterfaceClass::reportSwitch(uint16_t addr) {
}

void XpressNetInterfaceClass::sendSpeed(uint16_t addr, uint8_t speed) {
}

void XpressNetInterfaceClass::sendDirFunct0to4(uint8_t id, uint8_t direction, uint16_t functions) {
  uint8_t dirFunct0to4 = (direction << 5) | ((functions & 0x1) << 4) | ((functions & 0x1E) >> 1);
  DIAG("XpressNetInterfaceClass::sendDirFunct0to4 id:%u, dir:%u, func:%u\n", id, direction, functions);
}

void XpressNetInterfaceClass::sendFunct5to8(uint8_t id, uint16_t functions) {
  uint8_t funct5to8 = ((functions & 0x1E0) >> 5);
}

void XpressNetInterfaceClass::sendSwitch(uint16_t addr, uint8_t out, uint8_t dir) {
}

void XpressNetInterfaceClass::sendSensor(uint16_t addr, uint8_t state) {
}*/
  
void XpressNetInterfaceClass::setPower(uint8_t power) {
  power==PowerOff ? XpressNetMasterClass::setPower(csTrackVoltageOff) : XpressNetMasterClass::setPower(csNormal);
}

void XpressNetInterfaceClass::EmergencyStop(uint8_t src) {
  XpressNetMasterClass::setPower(csEmergencyStop);
}

XpressNetInterfaceClass XpressNetInterface;

/*void notifyXNetgiveLocoInfo(uint8_t UserOps, uint16_t Address) {
    XpressNet.SetLocoInfo(UserOps, 0x00, 0x00, 0x00); //UserOps,Speed,F0,F1
  }
  
  void notifyXNetgiveLocoFunc(uint8_t UserOps, uint16_t Address) {
    XpressNet.SetFktStatus(UserOps, 0x00, 0x00); //Fkt4, Fkt5
  }*/
  
  
  
void notifyXNetPower(uint8_t State) {
  switch (State) {
    case csNormal:
      CommandStation.setPower(PowerOn, SRC_XPRESSNET);
      break;
    case csTrackVoltageOff:
      CommandStation.setPower(PowerOff, SRC_XPRESSNET);
      break;
    case csEmergencyStop:
      CommandStation.EmergencyStop(SRC_XPRESSNET);
      break;
  }
}
  
  //--------------------------------------------------------------
  void notifyXNetLocoDrive14(uint16_t Address, uint8_t Speed) {
    Serial.print("XNet A:");
    Serial.print(Address);
    Serial.print(", S14:");
    Serial.println(Speed, BIN);
  }
  
  //--------------------------------------------------------------
  void notifyXNetLocoDrive28(uint16_t Address, uint8_t Speed) {
    Serial.print("XNet A:");
    Serial.print(Address);
    Serial.print(", S28:");
    Serial.println(Speed, BIN);
  }
  
  //--------------------------------------------------------------
  void notifyXNetLocoDrive128(uint16_t Address, uint8_t Speed) {
    Serial.print("XNet A:");
    Serial.print(Address);
    Serial.print(", S128:");
    Serial.println(Speed, BIN);
  }
  
  //--------------------------------------------------------------
  void notifyXNetLocoFunc1(uint16_t Address, uint8_t Func1) {
    Serial.print("XNet A:");
    Serial.print(Address);
    Serial.print(", F1:");
    Serial.println(Func1, BIN);
  }
  
  //--------------------------------------------------------------
  void notifyXNetLocoFunc2(uint16_t Address, uint8_t Func2) {
    Serial.print("XNet A:");
    Serial.print(Address);
    Serial.print(", F2:");
    Serial.println(Func2, BIN);
  }
  
  //--------------------------------------------------------------
  void notifyXNetLocoFunc3(uint16_t Address, uint8_t Func3) {
    Serial.print("XNet A:");
    Serial.print(Address);
    Serial.print(", F3:");
    Serial.println(Func3, BIN);
  }
  
  //--------------------------------------------------------------
  void notifyXNetLocoFunc4(uint16_t Address, uint8_t Func4) {
    Serial.print("XNet A:");
    Serial.print(Address);
    Serial.print(", F4:");
    Serial.println(Func4, BIN);
  }
  
  //--------------------------------------------------------------
  void notifyXNetLocoFunc5(uint16_t Address, uint8_t Func5) {
    Serial.print("XNet A:");
    Serial.print(Address);
    Serial.print(", F5:");
    Serial.println(Func5, BIN);
  }
  void notifyXNetTrnt(uint16_t Address, uint8_t data) {
      Serial.print("XNet TA:");
      Serial.print(Address);
      Serial.print(", P:");
      Serial.print(data & 0x01);
      Serial.print(" - Aktiv=");
      Serial.println(bitRead(data,3));
  }
  
  void notifyXNetTrntInfo(uint8_t UserOps, uint8_t Address, uint8_t data) {
    int adr = ((Address * 4) + ((data & 0x01) * 2));
    byte pos = data << 4;
    bitWrite(pos, 7, 1);  //command completed!
  /*  
    if (dcc.getBasicAccessoryInfo(adr) == false)
      bitWrite(pos, 0, 1);
    else bitWrite(pos, 1, 1);  
    if (dcc.getBasicAccessoryInfo(adr+1) == false)
      bitWrite(pos, 2, 1);  
    else bitWrite(pos, 3, 1);    
  */  
  //  XpressNet.SetTrntStatus(UserOps, Address, pos);
  }