#ifndef __CREATE_CLAN_H__
#define __CREATE_CLAN_H__

#include <Geode/Geode.hpp>

class CreateClan : public cocos2d::CCLayer {
public:
    void onProfileButton(cocos2d::CCObject* sender);
    void onCreateClan(cocos2d::CCObject* sender);
    void onPrivPublic(cocos2d::CCObject* sender);
    void onPrivInvite(cocos2d::CCObject* sender);
    void onPrivRequest(cocos2d::CCObject* sender);
    void onEditBadge(cocos2d::CCObject* sender);
    void onInfoButton(cocos2d::CCObject* sender);
    void onClanGuidelines(cocos2d::CCObject* sender);
    void onBadgeChange(cocos2d::CCObject* sender);
    void onBadgeBGChanged(cocos2d::CCObject* sender);
    void onBadgeExtraChanged(cocos2d::CCObject* sender);
    void onColorBox1(cocos2d::CCObject* sender);
    void onColorBox2(cocos2d::CCObject* sender);
    void colorValueChanged(cocos2d::ccColor4B const& color, int boxId);

};


#endif
