#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "BatteryStatus.hpp"

using namespace geode::prelude;

class $modify(StatusMenuLayer, MenuLayer) {

    bool init() {
        if (!MenuLayer::init()) return false;

        auto scene = CCDirector::sharedDirector()->getRunningScene();

        if (scene && !scene->getChildByID("battery-status")) {
            if (auto status = BatteryStatus::create()) {
                status->setID("battery-status");
                status->setPosition({0, 0});
                this->addChild(status);
                SceneManager::get()->keepAcrossScenes(status);

                log::debug("hi from battery status");
            }
        }

        return true;
    }
};