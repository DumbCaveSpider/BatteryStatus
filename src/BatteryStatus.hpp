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

    cocos2d::CCNode *m_lowBat;
    cocos2d::CCNode *m_halfBat;
    cocos2d::CCNode *m_fullBat;
    cocos2d::CCNode *m_chargingBat;
    cocos2d::CCNode *m_emptyBat;

    // last known state
    int m_lastLevel;
    bool m_lastCharging;
    bool m_lastSaver;

    // scheduled update
    void updateBattery(float dt);

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