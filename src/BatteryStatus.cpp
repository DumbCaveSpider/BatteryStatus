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
    auto lowBat = CCSprite::create("low.png"_spr);
    auto halfBat = CCSprite::create("half.png"_spr);
    auto fullBat = CCSprite::create("full.png"_spr);
    auto chargingBat = CCSprite::create("charging.png"_spr);
    auto emptyBat = CCSprite::create("empty.png"_spr);

    // check battery status
    log::debug("Battery level : {}", getBatteryLevel());
    log::debug("Is charging? : {}", isCharging());
    log::debug("Is battery saver? : {}", isBatterySaver());

    // display battery status icon
    if (isCharging()) {
        addChild(chargingBat);
        chargingBat->setPosition({winSize.width - 50, winSize.height - 50});
    }
    else {
        int level = getBatteryLevel();
        if (level >= 75) {
            addChild(fullBat);
            fullBat->setPosition({winSize.width - 50, winSize.height - 50});
        }
        else if (level >= 30) {
            addChild(halfBat);
            halfBat->setPosition({winSize.width - 50, winSize.height - 50});
        }
        else if (level > 0) {
            addChild(lowBat);
            lowBat->setPosition({winSize.width - 50, winSize.height - 50});
        }
        else {
            addChild(emptyBat);
            emptyBat->setPosition({winSize.width - 50, winSize.height - 50});
        }
    }

    return true;
}