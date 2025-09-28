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
    // show a text label with battery percentage and charging status
    auto label = CCLabelBMFont::create(fmt::format("Battery: {}", getBatteryLevel()).c_str(), "bigFont.fnt");
    label->setPosition({winSize.width / 2, winSize.height / 2});
    label->setScale(0.5f);
    addChild(label);

    return true;
}