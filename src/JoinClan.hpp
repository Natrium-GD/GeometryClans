#ifndef __JOIN_CLAN_H__
#define __JOIN_CLAN_H__

#pragma once

#include <Geode/Geode.hpp>
#include <cocos2d.h>
#include <Geode/cocos/particle_nodes/CCParticleSystemQuad.h>

using namespace geode::prelude;
using namespace cocos2d;

class JoinClan : public CCLayer {
protected:
    bool init() override;
    void keyBackClicked() override;
    void onGoBack(CCObject* sender);

public:
    static JoinClan* create();
    static CCScene* switchToScene();

    void onCreate(cocos2d::CCObject* sender);
    // void onPageBack(cocos2d::CCObject* sender);
    // void onPageNext(cocos2d::CCObject* sender);
    void onRefresh(cocos2d::CCObject* sender);
    void onSearch(cocos2d::CCObject* sender);
    void onFilter(cocos2d::CCObject* sender);
    // void onPageButton(cocos2d::CCObject* sender);
    void onViewClan(cocos2d::CCObject* sender);

};


#endif