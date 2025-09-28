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

    CCSprite *m_lowBat;
    CCSprite *m_halfBat;
    CCSprite *m_fullBat;
    CCSprite *m_chargingBat;
    CCSprite *m_emptyBat;

    CCLabelBMFont *m_percentageLabel;

    // last known state
    int m_lastLevel;
    bool m_lastCharging;
    bool m_lastSaver;
    bool m_initialized;

    // battery status code:
    // 0 = unknown/uninitialized
    // 1 = empty battery
    // 2 = low battery
    // 3 = half battery
    // 4 = full battery
    int m_status;

    // scheduled update
    void updateBattery(float dt);
    void onEnter() override;

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