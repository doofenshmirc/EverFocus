#ifndef DCCEXInterface_h
#define DCCEXInterface_h

#include "WiFi.h"
#include "LiquidCrystal_I2C.h"
#include <DCCEXProtocol.h>
#include "CommandStation.h"
#include "diag.h"

class DCCEXInterfaceClass : public DCCEXProtocolDelegate {
  public:
    int trackAAddr = DCCEX_TRACK_A_ADDR;
    uint8_t trackAType = DCCEX_TRACK_A_TYPE;
    int trackBAddr = DCCEX_TRACK_B_ADDR;
    uint8_t trackBType = DCCEX_TRACK_B_TYPE;
    int trackCAddr = DCCEX_TRACK_C_ADDR;
    uint8_t trackCType = DCCEX_TRACK_C_TYPE;
    int trackDAddr = DCCEX_TRACK_D_ADDR;
    uint8_t trackDType = DCCEX_TRACK_D_TYPE;
    int dccExAddress[4] = DCCEX_ADDRESS;
    int dccExPort = DCCEX_PORT;

    DCCEXInterfaceClass();
    void init(LiquidCrystal_I2C *lcd, bool wifi);
    void receivedMessage(char *message);
    void receivedServerVersion(int major, int minor, int patch) override;
    void receivedTrackPower(TrackPower state) override;
    void receivedTrackType(char track, TrackManagerMode mode, int addr) override;
    void receivedRosterList() override;
    void receivedLocoUpdate(Loco *loco) override;
    void receivedLocoBroadcast(int addr, int speed, Direction direction, int functionMap) override;
    void receivedSensorState(int addr, bool state);
    void setThrottle(uint16_t addr, uint8_t speed, uint8_t dir);
    void setFunctions(uint16_t addr, uint16_t functions, uint16_t funct_status);
    void setSwitch(uint16_t addr, uint8_t out, uint8_t dir);
    void setPower(uint8_t power);
    void requestTracksType();
    void requestRosterList();
    Loco* getRosterList() { return _dccex.roster; };
    void setTracksType();
    void EmergencyStop(uint8_t src);
    void saveConfig();
    void loadConfig();
    void check();
  private:
    WiFiClient _wifiClient;
    DCCEXProtocol _dccex;
};

extern DCCEXInterfaceClass DCCEXInterface;

#endif