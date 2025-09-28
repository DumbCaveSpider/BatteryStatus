#include <Geode/Geode.hpp>
#include <Geode/modify/CCMenu.hpp>

using namespace geode::prelude;

class BatteryInfo
{
public:
    int level;
    bool charging;
    bool batterySaver;
};

class BatteryStatus : public CCMenu
{
protected:
    bool init() override;
    static BatteryStatus *instance;

public:
    BatteryStatus();
    ~BatteryStatus();
    static BatteryStatus *create();
    static BatteryStatus *get();
};

namespace battery
{
    static BatteryStatus *instance;
    static BatteryInfo *get();
    int getBatteryLevel();
    bool isCharging();
    bool isBatterySaver();
};