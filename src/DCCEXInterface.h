#ifndef DCCEXInterface_h
#define DCCEXInterface_h

#include <DCCEXProtocol.h>
#include "CommandStation.h"
#include "diag.h"

class DCCEXInterfaceClass : public DCCEXProtocolDelegate {
  public:
    int trackAAddr = 0;
    uint8_t trackAType = 0;
    int trackBAddr = 0;
    uint8_t trackBType = 1;
    int trackCAddr = 0;
    uint8_t trackCType = 4;
    int trackDAddr = 0;
    uint8_t trackDType = 4;

    DCCEXInterfaceClass();
    void init(Stream *client, Stream *log);
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
    void EmergencyStop();
    void check();
  private:
    DCCEXProtocol _dccex;
};

extern DCCEXInterfaceClass DCCEXInterface;

#endif