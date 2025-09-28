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
    // sprites
    m_lowBat = CCSprite::create("low.png"_spr);
    m_halfBat = CCSprite::create("half.png"_spr);
    m_fullBat = CCSprite::create("full.png"_spr);
    m_chargingBat = CCSprite::create("charging.png"_spr);
    m_emptyBat = CCSprite::create("empty.png"_spr);

    // init invisible
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

    // initialize status (0 = unknown)
    m_status = 0;

    // initialize percentage label
    m_percentageLabel = nullptr;

    return true;
}

void BatteryStatus::updateBattery(float dt)
{
    int level = battery::getBatteryLevel();
    bool charging = battery::isCharging();
    bool saver = battery::isBatterySaver();

    int lowThreshold = Mod::get()->getSettingValue<int>("lowThreshold");
    int mediumThreshold = Mod::get()->getSettingValue<int>("mediumThreshold");
    int highThreshold = Mod::get()->getSettingValue<int>("highThreshold");

    auto type = Mod::get()->getSettingValue<std::string>("displayStatus");
    auto pos = Mod::get()->getSettingValue<std::string>("position");
    bool enabled = Mod::get()->getSettingValue<bool>("enabled");

    // update last-known state
    m_lastLevel = level;
    m_lastCharging = charging;
    m_lastSaver = saver;

    // disabled
    if (!enabled)
        return;
    log::debug("BatteryStatus::updateBattery - level={} charging={} saver={} type={} pos={} status={}", level, charging, saver, type, pos, m_status);

    // show icon if the type is set to icon
    if (type == "Icon")
    {
        // hide percentage label if exists
        if (m_percentageLabel)
        {
            m_percentageLabel->setVisible(false);
        }

        // default colors
        m_fullBat->setColor({0, 255, 0});
        m_halfBat->setColor({255, 255, 0});
        m_lowBat->setColor({255, 165, 0});
        m_emptyBat->setColor({255, 0, 0});

        // hide all
        m_lowBat->setVisible(false);
        m_halfBat->setVisible(false);
        m_fullBat->setVisible(false);
        m_chargingBat->setVisible(false);
        m_emptyBat->setVisible(false);

        // 0 = unknown, 1 = empty, 2 = low, 3 = half, 4 = full
        if (level >= highThreshold)
        {
            m_status = 4;
            m_chargingBat->setColor({0, 255, 0});
        }
        else if (level >= mediumThreshold)
        {
            m_status = 3;
            m_chargingBat->setColor({255, 255, 0});
        }
        else if (level >= lowThreshold)
        {
            m_status = 2;
            m_chargingBat->setColor({255, 165, 0});
        }
        else if (level >= 0)
        {
            m_status = 1;
            m_chargingBat->setColor({255, 0, 0});
        }
        else
        { // -1 = unknown level
            m_status = -1;
            m_chargingBat->setColor({191, 191, 191});
        }

        // show the appropriate sprite for the status
        switch (m_status)
        {
        case 4:
            static_cast<CCSprite *>(m_fullBat)->setVisible(true);
            // hide others
            static_cast<CCSprite *>(m_halfBat)->setVisible(false);
            static_cast<CCSprite *>(m_lowBat)->setVisible(false);
            static_cast<CCSprite *>(m_chargingBat)->setVisible(false);
            static_cast<CCSprite *>(m_emptyBat)->setVisible(false);
            break;
        case 3:
            static_cast<CCSprite *>(m_halfBat)->setVisible(true);
            // hide others
            static_cast<CCSprite *>(m_lowBat)->setVisible(false);
            static_cast<CCSprite *>(m_chargingBat)->setVisible(false);
            static_cast<CCSprite *>(m_emptyBat)->setVisible(false);
            static_cast<CCSprite *>(m_fullBat)->setVisible(false);
            break;
        case 2:
            static_cast<CCSprite *>(m_lowBat)->setVisible(true);
            // hide others
            static_cast<CCSprite *>(m_chargingBat)->setVisible(false);
            static_cast<CCSprite *>(m_emptyBat)->setVisible(false);
            static_cast<CCSprite *>(m_fullBat)->setVisible(false);
            static_cast<CCSprite *>(m_halfBat)->setVisible(false);
            break;
        case 1:
            static_cast<CCSprite *>(m_emptyBat)->setVisible(true);
            // hide others
            static_cast<CCSprite *>(m_chargingBat)->setVisible(false);
            static_cast<CCSprite *>(m_fullBat)->setVisible(false);
            static_cast<CCSprite *>(m_halfBat)->setVisible(false);
            static_cast<CCSprite *>(m_lowBat)->setVisible(false);
            break;
        default:
            static_cast<CCSprite *>(m_emptyBat)->setVisible(true);
            // hide others
            static_cast<CCSprite *>(m_chargingBat)->setVisible(false);
            static_cast<CCSprite *>(m_fullBat)->setVisible(false);
            static_cast<CCSprite *>(m_halfBat)->setVisible(false);
            static_cast<CCSprite *>(m_lowBat)->setVisible(false);
            static_cast<CCSprite *>(m_emptyBat)->setColor({128, 128, 128});
            break;
        }

        log::debug("BatteryStatus::updateBattery - computed status={} (level={})", m_status, level);

        // if saver is on, change all of the colors to default color
        if (saver)
        {
            m_fullBat->setColor({255, 255, 255});
            m_halfBat->setColor({255, 255, 255});
            m_lowBat->setColor({255, 255, 255});
            m_emptyBat->setColor({255, 255, 255});
            m_chargingBat->setColor({255, 255, 255});
        }

        // if charging, show charging sprite on top but keep status derived from level
        if (charging)
        {
            m_chargingBat->setVisible(true);
            // hide others
            m_lowBat->setVisible(false);
            m_halfBat->setVisible(false);
            m_fullBat->setVisible(false);
            m_emptyBat->setVisible(false);
            return;
        }
    }
    else if (type == "Percentage")
    {
        // set percentage label
        if (!m_percentageLabel)
        {
            m_percentageLabel = CCLabelBMFont::create("Battery:\n0%", "bigFont.fnt");
            m_percentageLabel->setAlignment(kCCTextAlignmentCenter);
            this->addChild(m_percentageLabel);
        }

        // update label text every check
        if (m_percentageLabel)
        {
            m_percentageLabel->setString(fmt::format("Battery:\n{}%", level).c_str());
        }

        // change text color
        if (level >= highThreshold)
        {
            m_percentageLabel->setColor({0, 255, 0});
        }
        else if (level >= mediumThreshold)
        {
            m_percentageLabel->setColor({255, 255, 0});
        }
        else if (level >= lowThreshold)
        {
            m_percentageLabel->setColor({255, 165, 0});
        }
        else
        {
            m_percentageLabel->setColor({255, 0, 0});
        }

        // add charging or saver text is charging or saver is on
        if (saver)
        {
            m_percentageLabel->setString(fmt::format("Battery:\n{}%\n(Saver)", level).c_str());
        }
        if (charging)
        {
            m_percentageLabel->setString(fmt::format("Battery:\n{}%\n(Charging)", level).c_str());
        }
        if (charging && saver)
        {
            m_percentageLabel->setString(fmt::format("Battery:\n{}%\n(Charging, Saver)", level).c_str());
        }

        // if the level is unknown -1, show unknown text
        if (level == -1)
        {
            m_percentageLabel->setString("Battery:\nUnknown");
            m_percentageLabel->setColor({128, 128, 128});
        }
    }
};

void BatteryStatus::onEnter()
{
    CCMenu::onEnter();

    bool enabled = Mod::get()->getSettingValue<bool>("enabled");

    // completely disable if not enabled
    if (!enabled)
        return;

    // position for all sprites
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto pos = Mod::get()->getSettingValue<std::string>("position");
    int opacity = Mod::get()->getSettingValue<int>("opacity");
    float scale = Mod::get()->getSettingValue<float>("scale");
    auto type = Mod::get()->getSettingValue<std::string>("displayStatus");

    log::debug("BatteryStatus::onEnter - type={} position={} scale={} opacity={}", type, pos, scale, opacity);

    // hide everything first
    m_lowBat->setVisible(false);
    m_halfBat->setVisible(false);
    m_fullBat->setVisible(false);
    m_chargingBat->setVisible(false);
    m_emptyBat->setVisible(false);

    if (m_percentageLabel)
        m_percentageLabel->setVisible(false);
    // only if the type is icon
    if (type == "Icon")
    {
        if (pos == "Top Left")
        {
            m_lowBat->setPosition({10, winSize.height - 10});
            m_halfBat->setPosition({10, winSize.height - 10});
            m_fullBat->setPosition({10, winSize.height - 10});
            m_chargingBat->setPosition({10, winSize.height - 10});
            m_emptyBat->setPosition({10, winSize.height - 10});

            // set anchor point to top left
            m_lowBat->setAnchorPoint({0, 1});
            m_halfBat->setAnchorPoint({0, 1});
            m_fullBat->setAnchorPoint({0, 1});
            m_chargingBat->setAnchorPoint({0, 1});
            m_emptyBat->setAnchorPoint({0, 1});
        }
        else if (pos == "Top Right")
        {
            m_lowBat->setPosition({winSize.width - 10, winSize.height - 10});
            m_halfBat->setPosition({winSize.width - 10, winSize.height - 10});
            m_fullBat->setPosition({winSize.width - 10, winSize.height - 10});
            m_chargingBat->setPosition({winSize.width - 10, winSize.height - 10});
            m_emptyBat->setPosition({winSize.width - 10, winSize.height - 10});

            // set anchor point to top right
            m_lowBat->setAnchorPoint({1, 1});
            m_halfBat->setAnchorPoint({1, 1});
            m_fullBat->setAnchorPoint({1, 1});
            m_chargingBat->setAnchorPoint({1, 1});
            m_emptyBat->setAnchorPoint({1, 1});
        }
        else if (pos == "Bottom Left")
        {
            m_lowBat->setPosition({10, 10});
            m_halfBat->setPosition({10, 10});
            m_fullBat->setPosition({10, 10});
            m_chargingBat->setPosition({10, 10});
            m_emptyBat->setPosition({10, 10});

            // set anchor point to bottom left
            m_lowBat->setAnchorPoint({0, 0});
            m_halfBat->setAnchorPoint({0, 0});
            m_fullBat->setAnchorPoint({0, 0});
            m_chargingBat->setAnchorPoint({0, 0});
            m_emptyBat->setAnchorPoint({0, 0});
        }
        else if (pos == "Bottom Right")
        {
            m_lowBat->setPosition({winSize.width - 10, 10});
            m_halfBat->setPosition({winSize.width - 10, 10});
            m_fullBat->setPosition({winSize.width - 10, 10});
            m_chargingBat->setPosition({winSize.width - 10, 10});
            m_emptyBat->setPosition({winSize.width - 10, 10});
            // set anchor point to bottom right
            m_lowBat->setAnchorPoint({1, 0});
            m_halfBat->setAnchorPoint({1, 0});
            m_fullBat->setAnchorPoint({1, 0});
            m_chargingBat->setAnchorPoint({1, 0});
            m_emptyBat->setAnchorPoint({1, 0});
        }
        else if (pos == "Top Center")
        {
            m_lowBat->setPosition({winSize.width / 2, winSize.height - 10});
            m_halfBat->setPosition({winSize.width / 2, winSize.height - 10});
            m_fullBat->setPosition({winSize.width / 2, winSize.height - 10});
            m_chargingBat->setPosition({winSize.width / 2, winSize.height - 10});
            m_emptyBat->setPosition({winSize.width / 2, winSize.height / 2});

            // set anchor point to center
            m_lowBat->setAnchorPoint({0.5, 0.5});
            m_halfBat->setAnchorPoint({0.5, 0.5});
            m_fullBat->setAnchorPoint({0.5, 0.5});
            m_chargingBat->setAnchorPoint({0.5, 0.5});
            m_emptyBat->setAnchorPoint({0.5, 0.5});
        }
        else if (pos == "Center Left")
        {
            m_lowBat->setPosition({10, winSize.height / 2});
            m_halfBat->setPosition({10, winSize.height / 2});
            m_fullBat->setPosition({10, winSize.height / 2});
            m_chargingBat->setPosition({10, winSize.height / 2});
            m_emptyBat->setPosition({10, winSize.height / 2});

            // set anchor point to center left
            m_lowBat->setAnchorPoint({0, 0.5});
            m_halfBat->setAnchorPoint({0, 0.5});
            m_fullBat->setAnchorPoint({0, 0.5});
            m_chargingBat->setAnchorPoint({0, 0.5});
            m_emptyBat->setAnchorPoint({0, 0.5});
        }
        else if (pos == "Center Right")
        {
            m_lowBat->setPosition({winSize.width - 10, winSize.height / 2});
            m_halfBat->setPosition({winSize.width - 10, winSize.height / 2});
            m_fullBat->setPosition({winSize.width - 10, winSize.height / 2});
            m_chargingBat->setPosition({winSize.width - 10, winSize.height / 2});
            m_emptyBat->setPosition({winSize.width - 10, winSize.height / 2});

            // set anchor point to center right
            m_lowBat->setAnchorPoint({1, 0.5});
            m_halfBat->setAnchorPoint({1, 0.5});
            m_fullBat->setAnchorPoint({1, 0.5});
            m_chargingBat->setAnchorPoint({1, 0.5});
            m_emptyBat->setAnchorPoint({1, 0.5});
        }
        else if (pos == "Bottom Center")
        {
            m_lowBat->setPosition({winSize.width / 2, 10});
            m_halfBat->setPosition({winSize.width / 2, 10});
            m_fullBat->setPosition({winSize.width / 2, 10});
            m_chargingBat->setPosition({winSize.width / 2, 10});
            m_emptyBat->setPosition({winSize.width / 2, 10});

            // set anchor point to bottom center
            m_lowBat->setAnchorPoint({0.5, 0});
            m_halfBat->setAnchorPoint({0.5, 0});
            m_fullBat->setAnchorPoint({0.5, 0});
            m_chargingBat->setAnchorPoint({0.5, 0});
            m_emptyBat->setAnchorPoint({0.5, 0});
        }
        else
        {
            m_lowBat->setPosition({winSize.width - 10, winSize.height - 10});
            m_halfBat->setPosition({winSize.width - 10, winSize.height - 10});
            m_fullBat->setPosition({winSize.width - 10, winSize.height - 10});
            m_chargingBat->setPosition({winSize.width - 10, winSize.height - 10});
            m_emptyBat->setPosition({winSize.width - 10, winSize.height - 10});
        }

        // set scale
        m_lowBat->setScale(scale);
        m_halfBat->setScale(scale);
        m_fullBat->setScale(scale);
        m_chargingBat->setScale(scale);
        m_emptyBat->setScale(scale);

        // opacity
        m_lowBat->setOpacity(opacity);
        m_halfBat->setOpacity(opacity);
        m_fullBat->setOpacity(opacity);
        m_chargingBat->setOpacity(opacity);
        m_emptyBat->setOpacity(opacity);
    }
    else if (type == "Percentage")
    {
        // set percentage label
        if (!m_percentageLabel)
        {
            m_percentageLabel = CCLabelBMFont::create("0%", "bigFont.fnt");
            this->addChild(m_percentageLabel);
        }

        // make label visible
        m_percentageLabel->setVisible(true);

        // position label
        if (pos == "Top Left")
        {
            if (m_percentageLabel)
            {
                m_percentageLabel->setPosition({10, winSize.height - 10});
                m_percentageLabel->setAnchorPoint({0, 1});
            }
        }
        else if (pos == "Top Center")
        {
            if (m_percentageLabel)
            {
                m_percentageLabel->setPosition({winSize.width / 2, winSize.height - 10});
                m_percentageLabel->setAnchorPoint({0.5, 1});
            }
        }
        else if (pos == "Top Right")
        {
            if (m_percentageLabel)
            {
                m_percentageLabel->setPosition({winSize.width - 10, winSize.height - 10});
                m_percentageLabel->setAnchorPoint({1, 1});
            }
        }
        else if (pos == "Bottom Left")
        {
            if (m_percentageLabel)
            {
                m_percentageLabel->setPosition({10, 10});
                m_percentageLabel->setAnchorPoint({0, 0});
            }
        }
        else if (pos == "Bottom Right")
        {
            if (m_percentageLabel)
            {
                m_percentageLabel->setPosition({winSize.width - 10, 10});
                m_percentageLabel->setAnchorPoint({1, 0});
            }
        }
        else if (pos == "Bottom Center")
        {
            if (m_percentageLabel)
            {
                m_percentageLabel->setPosition({winSize.width / 2, 10});
                m_percentageLabel->setAnchorPoint({0.5, 0});
            }
        }
        else if (pos == "Center Left")
        {
            if (m_percentageLabel)
            {
                m_percentageLabel->setPosition({10, winSize.height / 2});
                m_percentageLabel->setAnchorPoint({0, 0.5});
            }
        }
        else if (pos == "Center Right")
        {
            if (m_percentageLabel)
            {
                m_percentageLabel->setPosition({winSize.width - 10, winSize.height / 2});
                m_percentageLabel->setAnchorPoint({1, 0.5});
            }
        }
        else if (pos == "Bottom Center")
        {
            if (m_percentageLabel)
            {
                m_percentageLabel->setPosition({winSize.width / 2, 10});
                m_percentageLabel->setAnchorPoint({0.5, 0});
            }
        }
        else
        {
            if (m_percentageLabel)
            {
                m_percentageLabel->setPosition({winSize.width - 10, winSize.height - 10});
                m_percentageLabel->setAnchorPoint({1, 1});
            }
        }
        // set scale and opacity
        if (m_percentageLabel)
        {
            m_percentageLabel->setScale(scale);
            m_percentageLabel->setOpacity(opacity);
        }
        // run an immediate update and schedule will be performed below for all types
    }

    // run an immediate update then schedule periodic checks every 1 second
    float checkRate = Mod::get()->getSettingValue<float>("checkRate");
    updateBattery(0);
    this->schedule(schedule_selector(BatteryStatus::updateBattery), checkRate);
}