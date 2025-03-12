#ifndef ICON_CREATOR_H
#define ICON_CREATOR_H

#include <Geode/Geode.hpp>
#include <iostream>
#include <sstream>
#include <string>

using namespace geode::prelude;

class IconCreator {
public:
    SimplePlayer* buildIcon(int cubeID, std::string iconColors, bool glow) {

        auto icon = SimplePlayer::create(cubeID);

        auto gm = GameManager::get();

        int col1, col2, col3;
        std::string temp;
        std::stringstream ss(iconColors);
        std::getline(ss, temp, ';'); 
        col1 = std::stoi(temp); 
        std::getline(ss, temp, ';'); 
        col2 = std::stoi(temp); 
        std::getline(ss, temp, ';');
        col3 = std::stoi(temp);

        cocos2d::ccColor3B color1 = gm->colorForIdx(col1);
        cocos2d::ccColor3B color2 = gm->colorForIdx(col2);
        cocos2d::ccColor3B color3 = gm->colorForIdx(col3);

        icon->setColors(color1, color2);
        icon->setGlowOutline(color3);

        if (!glow) icon->disableGlowOutline();

        return icon;
    }
};

#endif
