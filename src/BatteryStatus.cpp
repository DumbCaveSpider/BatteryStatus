#include <Geode/Geode.hpp>
#include <Geode/modify/CCScene.hpp>
#include <Geode/modify/CCMenu.hpp>
#include "BatteryStatus.hpp"

using namespace geode::prelude;
using namespace battery;

BatteryStatus *BatteryStatus::instance = nullptr;

BatteryStatus::BatteryStatus()
{
    log::debug("BatteryStatus constructor");
}

BatteryStatus::~BatteryStatus()
{
    log::debug("BatteryStatus destructor");
}

BatteryStatus *BatteryStatus::get()
{
    if (!instance)
    {
        instance = new BatteryStatus();
    }
    return instance;
}

BatteryStatus *BatteryStatus::create()
{
    auto ret = new BatteryStatus();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool BatteryStatus::init()
{
    if (!CCMenu::init())
        return false;

    auto scene = CCDirector::sharedDirector()->getRunningScene();
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    // sprites
    m_lowBat = CCSprite::create("low.png"_spr);
    m_halfBat = CCSprite::create("half.png"_spr);
    m_fullBat = CCSprite::create("full.png"_spr);
    m_chargingBat = CCSprite::create("charging.png"_spr);
    m_emptyBat = CCSprite::create("empty.png"_spr);

    // position all sprites, but only add once and toggle visibility
    auto pos = ccp(winSize.width - 50, winSize.height - 50);
    m_lowBat->setPosition(pos);
    m_halfBat->setPosition(pos);
    m_fullBat->setPosition(pos);
    m_chargingBat->setPosition(pos);
    m_emptyBat->setPosition(pos);

    // add children (initially invisible)
    addChild(m_lowBat);
    addChild(m_halfBat);
    addChild(m_fullBat);
    addChild(m_chargingBat);
    addChild(m_emptyBat);

    m_lowBat->setVisible(false);
    m_halfBat->setVisible(false);
    m_fullBat->setVisible(false);
    m_chargingBat->setVisible(false);
    m_emptyBat->setVisible(false);

    // initialize last known state
    m_lastLevel = -1;
    m_lastCharging = false;
    m_lastSaver = false;

    // run an immediate update then schedule periodic checks every 1 second
    updateBattery(0);
    this->schedule(schedule_selector(BatteryStatus::updateBattery), 1.0f);

    return true;
}

void BatteryStatus::updateBattery(float dt)
{
    int level = battery::getBatteryLevel();
    bool charging = battery::isCharging();
    bool saver = battery::isBatterySaver();

    // if nothing changed, do nothing
    if (level == m_lastLevel && charging == m_lastCharging && saver == m_lastSaver)
        return;

    m_lastLevel = level;
    m_lastCharging = charging;
    m_lastSaver = saver;

    // hide all
    m_lowBat->setVisible(false);
    m_halfBat->setVisible(false);
    m_fullBat->setVisible(false);
    m_chargingBat->setVisible(false);
    m_emptyBat->setVisible(false);

    if (charging)
    {
        m_chargingBat->setVisible(true);
        return;
    }

    if (level >= 75)
    {
        m_fullBat->setVisible(true);
    }
    else if (level >= 30)
    {
        m_halfBat->setVisible(true);
    }
    else if (level > 0)
    {
        m_lowBat->setVisible(true);
    }
    else
    {
        m_emptyBat->setVisible(true);
    }
}