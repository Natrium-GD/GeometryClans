#include <cocos2d.h>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/cocos/particle_nodes/CCParticleSystemQuad.h>

#include "WebRequestHelper.hpp"
#include "globals.hpp"
#include "JoinClan.hpp"

using namespace geode::prelude;

struct Fields {
    EventListener<web::WebTask> m_listener;
    int currentPage = 1;
};

Fields joinClanFields;


bool JoinClan::init() {
    auto dir = CCDirector::get();
    this->setKeypadEnabled(true);
    auto menu = CCMenu::create();
    menu->setPosition({ 0, 0 });

    auto bgSprite = CCSprite::create("GJ_gradientBG.png");
    bgSprite->setVisible(true);
    bgSprite->setScaleX((dir->getWinSize().width) / bgSprite->getTextureRect().size.width);
    bgSprite->setScaleY(dir->getWinSize().height / bgSprite->getTextureRect().size.height);
    bgSprite->setAnchorPoint({ 0,0 });
    bgSprite->setPosition({ 0,0 });
    bgSprite->setColor(ccColor3B(0, 102, 255));

    auto closeButtonSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    auto closeButton = CCMenuItemSpriteExtra::create(closeButtonSprite, this, menu_selector(JoinClan::onGoBack));
    closeButton->setPosition(26, 295);
    menu->addChild(closeButton);

    // Remove for the time being, used for page system
   /* auto previousSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    auto previousPageButton = CCMenuItemSpriteExtra::create(previousSprite, this, menu_selector(JoinClan::onPageBack));
    previousPageButton->setPosition(26, (dir->getWinSize().height) / 2);
    menu->addChild(previousPageButton);
    previousPageButton->setVisible(false);

    auto nextSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    nextSprite->setFlipX(true);
    auto nextPageButton = CCMenuItemSpriteExtra::create(nextSprite, this, menu_selector(JoinClan::onPageNext));
    nextPageButton->setPosition((dir->getWinSize().width) - 26, (dir->getWinSize().height) / 2);
    menu->addChild(nextPageButton); */

    auto refreshSprite = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    auto refreshButton = CCMenuItemSpriteExtra::create(refreshSprite, this, menu_selector(JoinClan::onRefresh));
    refreshButton->setPosition(540, 29);
    menu->addChild(refreshButton);

    auto searchSprite = CCSprite::createWithSpriteFrameName("gj_findBtn_001.png");
    auto searchButton = CCMenuItemSpriteExtra::create(searchSprite, this, menu_selector(JoinClan::onSearch));
    searchButton->setPosition(520, 245);
    menu->addChild(searchButton);

    auto filterSprite = CCSprite::create("GJ_button_05.png");
    filterSprite->setScale(0.8f);
    auto filterSpriteExtra = CCSprite::createWithSpriteFrameName("GJ_filterIcon_001.png");
    filterSpriteExtra->setPosition(CCPoint(19.5f, 19.5f));
    auto filterButton = CCMenuItemSpriteExtra::create(filterSprite, this, menu_selector(JoinClan::onFilter));
    filterSprite->addChild(filterSpriteExtra);
    filterButton->setPosition(520, 210);
    menu->addChild(filterButton);

    /*
    auto pageButtonSprite = CCSprite::create("GJ_button_02.png");
    pageButtonSprite->setScale(0.75f);
    auto pageButton = CCMenuItemSpriteExtra::create(pageButtonSprite, this, menu_selector(JoinClan::onPageButton));
    pageButton->setPosition(546, 275);
    auto pageLabel = CCLabelBMFont::create(std::to_string(joinClanFields.currentPage).c_str(), "bigFont.fnt");
    pageLabel->setScale(0.8f);
    pageLabel->setPosition(19.5f, 21);
    pageButtonSprite->addChild(pageLabel);
    menu->addChild(pageButton);*/

    int clanAmount = 0;
    if (g_sheetData.contains("Clans") && g_sheetData["Clans"].isArray()) {
        clanAmount = g_sheetData["Clans"].size() - 1;
    }

    // std::string clanCountString = "1 to 10 of " + std::to_string(clanAmount);
    std::string clanCountString = "Total: " + std::to_string(clanAmount);

    auto clanCountLabel = CCLabelBMFont::create(clanCountString.c_str(), "goldFont.fnt");
    clanCountLabel->setAnchorPoint(CCPoint(1, 1));
    clanCountLabel->setPosition((dir->getWinSize().width) - 7, (dir->getWinSize().height) - 3);
    clanCountLabel->setScale(0.5f);

    auto& clansArray = g_sheetData["Clans"];
    auto& usersArray = g_sheetData["Users"];

    std::vector<std::string> clanTags;
    for (size_t i = 1; i < clansArray.size(); i++) {
        if (clansArray[i].isArray() && clansArray[i].size() > 2) {
            auto tagResult = clansArray[i][2].as<std::string>();
            if (tagResult) {
                clanTags.push_back(tagResult.unwrap());
            }
        }
    }

    std::vector<std::string> clanNames;
    for (size_t i = 1; i < clansArray.size(); i++) {
        if (clansArray[i].isArray() && clansArray[i].size() > 2) {
            auto tagResult = clansArray[i][3].as<std::string>();
            if (tagResult) {
                clanNames.push_back(tagResult.unwrap());
            }
        }
    }

    std::vector<int> clanBadgeBGShape;
    for (size_t i = 1; i < clansArray.size(); i++) {
        if (clansArray[i].isArray() && clansArray[i].size() > 6) {
            auto tagResult = clansArray[i][6].as<int>();
            int tagValue = tagResult.unwrap();
            clanBadgeBGShape.push_back(tagValue);
        }
    }

    std::vector<int> clanBadgeIconShape;
    for (size_t i = 1; i < clansArray.size(); i++) {
        if (clansArray[i].isArray() && clansArray[i].size() > 6) {
            auto tagResult = clansArray[i][8].as<int>();
            int tagValue = tagResult.unwrap();
            clanBadgeIconShape.push_back(tagValue);
        }
    }

    // For god knows what reason the values are sometimes sent in different orders (so not rgb, but grb instead)
    std::vector<ccColor3B> clanBadgeBGColor;
    for (size_t i = 1; i < clansArray.size(); i++) {
        if (clansArray[i].isArray() && clansArray[i].size() > 2) {
            auto tagResult = clansArray[i][7].as<std::string>();
            if (tagResult) {
                std::string colorString = tagResult.unwrap();
                float r = 0.0f, g = 0.0f, b = 0.0f;

                if (sscanf(colorString.c_str(), "{r=%f, g=%f, b=%f}", &r, &g, &b) == 3) {
                    ccColor3B color = { static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b) };
                    clanBadgeBGColor.push_back(color);
                }
                else if (sscanf(colorString.c_str(), "{r=%f, b=%f, g=%f}", &r, &b, &g) == 3) {
                    ccColor3B color = { static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b) };
                    clanBadgeBGColor.push_back(color);
                }
                else if (sscanf(colorString.c_str(), "{g=%f, r=%f, b=%f}", &g, &r, &b) == 3) {
                    ccColor3B color = { static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b) };
                    clanBadgeBGColor.push_back(color);
                }
                else if (sscanf(colorString.c_str(), "{g=%f, b=%f, r=%f}", &g, &b, &r) == 3) {
                    ccColor3B color = { static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b) };
                    clanBadgeBGColor.push_back(color);
                }
                else if (sscanf(colorString.c_str(), "{b=%f, r=%f, g=%f}", &b, &r, &g) == 3) {
                    ccColor3B color = { static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b) };
                    clanBadgeBGColor.push_back(color);
                }
                else if (sscanf(colorString.c_str(), "{b=%f, g=%f, r=%f}", &b, &g, &r) == 3) {
                    ccColor3B color = { static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b) };
                    clanBadgeBGColor.push_back(color);
                }
                else {
                    size_t pos = 0;
                    while ((pos = colorString.find(",", pos)) != std::string::npos) {
                        colorString.replace(pos, 1, "");
                    }
                    if (sscanf(colorString.c_str(), "{r=%f g=%f b=%f}", &r, &g, &b) == 3) {
                        ccColor3B color = { static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b) };
                        clanBadgeBGColor.push_back(color);
                    }
                }
            }
        }
    }


    std::vector<ccColor3B> clanBadgeIconColor;
    for (size_t i = 1; i < clansArray.size(); i++) {
        if (clansArray[i].isArray() && clansArray[i].size() > 2) {
            auto tagResult = clansArray[i][9].as<std::string>();
            if (tagResult) {
                std::string colorString = tagResult.unwrap();
                float r = 0.0f, g = 0.0f, b = 0.0f;

                if (sscanf(colorString.c_str(), "{r=%f, g=%f, b=%f}", &r, &g, &b) == 3) {
                    ccColor3B color = { static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b) };
                    clanBadgeIconColor.push_back(color);
                }
                else if (sscanf(colorString.c_str(), "{r=%f, b=%f, g=%f}", &r, &b, &g) == 3) {
                    ccColor3B color = { static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b) };
                    clanBadgeIconColor.push_back(color);
                }
                else if (sscanf(colorString.c_str(), "{g=%f, r=%f, b=%f}", &g, &r, &b) == 3) {
                    ccColor3B color = { static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b) };
                    clanBadgeIconColor.push_back(color);
                }
                else if (sscanf(colorString.c_str(), "{g=%f, b=%f, r=%f}", &g, &b, &r) == 3) {
                    ccColor3B color = { static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b) };
                    clanBadgeIconColor.push_back(color);
                }
                else if (sscanf(colorString.c_str(), "{b=%f, r=%f, g=%f}", &b, &r, &g) == 3) {
                    ccColor3B color = { static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b) };
                    clanBadgeIconColor.push_back(color);
                }
                else if (sscanf(colorString.c_str(), "{b=%f, g=%f, r=%f}", &b, &g, &r) == 3) {
                    ccColor3B color = { static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b) };
                    clanBadgeIconColor.push_back(color);
                }
                else {
                    size_t pos = 0;
                    while ((pos = colorString.find(",", pos)) != std::string::npos) {
                        colorString.replace(pos, 1, "");
                    }

                    if (sscanf(colorString.c_str(), "{r=%f g=%f b=%f}", &r, &g, &b) == 3) {
                        ccColor3B color = { static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b) };
                    }
                }
            }
        }
    }



    std::vector<std::string> clanPrivacy;
    for (size_t i = 1; i < clansArray.size(); i++) {
        if (clansArray[i].isArray() && clansArray[i].size() > 2) {
            auto tagResult = clansArray[i][5].as<std::string>();
            if (tagResult) {
                clanPrivacy.push_back(tagResult.unwrap());
            }
        }
    }

    std::vector<int> clanID;
    for (size_t i = 1; i < clansArray.size(); i++) {
        if (clansArray[i].isArray() && clansArray[i].size() > 6) {
            auto tagResult = clansArray[i][1].as<int>();
            int tagValue = tagResult.unwrap();
            clanID.push_back(tagValue);
        }
    }


    std::vector<std::string> badgeFilenames = {
    "badge_bg_1.png"_spr, "badge_bg_2.png"_spr, "badge_bg_3.png"_spr, "badge_bg_4.png"_spr, "badge_bg_6.png"_spr
    };

    std::vector<std::string> badgeHighlighFilenames = {
    "badge_bg_1_highlight.png"_spr, "badge_bg_2_highlight.png"_spr, "badge_bg_3_highlight.png"_spr, "badge_bg_4_highlight.png"_spr, "badge_bg_6_highlight.png"_spr
    };

    std::vector<std::string> badgeExtraFilenames = {
    "badge_icon_1.png"_spr, "badge_icon_2.png"_spr, "badge_icon_3.png"_spr,
    "badge_icon_4.png"_spr, "badge_icon_5.png"_spr, "badge_icon_6.png"_spr,
    "badge_icon_7.png"_spr, "badge_icon_8.png"_spr, "badge_icon_9.png"_spr,
    "badge_icon_10.png"_spr
    };

    std::vector<std::string> badgeExtraHighlighFilenames = {
    "badge_icon_1_highlight.png"_spr, "badge_icon_2_highlight.png"_spr, "badge_icon_3_highlight.png"_spr,
    "badge_icon_4_highlight.png"_spr, "badge_icon_5_highlight.png"_spr, "badge_icon_6_highlight.png"_spr,
    "badge_icon_7_highlight.png"_spr, "badge_icon_8_highlight.png"_spr, "badge_icon_9_highlight.png"_spr,
    "badge_icon_10_highlight.png"_spr
    };

    auto clansList = CCArray::create();
    // If decide to readd pages
    // for (int i = 0; i < std::min(10, clanAmount); i++) { 
    for (int i = 0; i < clanAmount; i++) {
        auto cell = CCLayer::create();

        std::string clanName = "[" + clanTags[i] + "] " + clanNames[i];

        float scale = 1.f;
        if (clanName.length() > 15) {
            if (clanName.length() >= 22) {
                scale = 0.75f;
            }
            else {
                float t = static_cast<float>(clanName.length() - 15) / (22 - 15);
                scale = 1.f - t * (1.f - 0.75f);
            }
        }

        auto nameLabel = CCLabelBMFont::create(clanName.c_str(), "goldFont.fnt");
        nameLabel->setAnchorPoint(CCPoint(0, 0.5));
        nameLabel->setPosition(70, 50);
        nameLabel->setScale(scale);
        cell->addChild(nameLabel);


        auto badgeSprite = CCSprite::create(badgeFilenames[clanBadgeBGShape[i] - 1].c_str());
        auto badgeSpriteHighlight = CCSprite::create(badgeHighlighFilenames[clanBadgeBGShape[i] - 1].c_str());
        auto badgeSpriteExtra = CCSprite::create(badgeExtraFilenames[clanBadgeIconShape[i] - 1].c_str());
        auto badgeSpriteExtraHighlight = CCSprite::create(badgeExtraHighlighFilenames[clanBadgeIconShape[i] - 1].c_str());

        badgeSprite->setColor(clanBadgeBGColor[i]);
        badgeSpriteExtra->setColor(clanBadgeIconColor[i]);

        badgeSprite->setPosition(CCPoint(35, 35));
        badgeSpriteHighlight->setPosition(CCPoint(35, 35));
        badgeSpriteExtra->setPosition(CCPoint(35, 35));
        badgeSpriteExtraHighlight->setPosition(CCPoint(35, 35));
        cell->addChild(badgeSprite);
        cell->addChild(badgeSpriteHighlight);
        cell->addChild(badgeSpriteExtra);
        cell->addChild(badgeSpriteExtraHighlight);

        auto menuCell = CCMenu::create(nullptr);

        auto viewClanSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");

        auto viewClanButton = CCMenuItemSpriteExtra::create(viewClanSprite, this, menu_selector(JoinClan::onViewClan));
        viewClanButton->setUserData(reinterpret_cast<void*>(static_cast<intptr_t>(clanID[i]))); // Store int as void*

        viewClanSprite->setFlipX(true);
        viewClanButton->setPosition(330, 35);
        menuCell->addChild(viewClanButton);
        menuCell->setPosition(0, 0);
        cell->addChild(menuCell);

        auto memberCountSprite = CCSprite::createWithSpriteFrameName("GJ_sFriendsIcon_001.png");
        memberCountSprite->setPosition(CCPoint(80, 20));
        memberCountSprite->setScale(1.2f);
        cell->addChild(memberCountSprite);

        int memberCount = 0; 
        std::string currentTag = clanTags[i];

        for (size_t j = 0; j < usersArray.size(); j++) {
            if (usersArray[j].isArray() && usersArray[j].size() > 3) {
                auto userTagResult = usersArray[j][3].as<std::string>(); 
                if (userTagResult) {
                    std::string userTag = userTagResult.unwrap();
                    if (userTag == clanTags[i]) {
                        memberCount++;
                    }
                }
            }
        }

        std::string memberCountString = std::to_string(memberCount) + "/30";
        auto memberCountLabel = CCLabelBMFont::create(memberCountString.c_str(), "bigFont.fnt");
        memberCountLabel->setColor(ccColor3B(0, 255, 0));
        memberCountLabel->setAnchorPoint(CCPoint(0, 0.5));
        memberCountLabel->setPosition(95, 20);
        memberCountLabel->setScale(0.5f);
        cell->addChild(memberCountLabel);

        auto privacySprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        privacySprite->setPosition(CCPoint(170, 20));
        privacySprite->setScale(0.75f);
        cell->addChild(privacySprite);

        auto privacyLabel = CCLabelBMFont::create(clanPrivacy[i].c_str(), "bigFont.fnt");
        privacyLabel->setAnchorPoint(CCPoint(0, 0.5));
        privacyLabel->setPosition(185, 20);
        privacyLabel->setScale(0.5f);
        cell->addChild(privacyLabel);

        clansList->addObject(cell);
    }

    auto list = ListView::create(clansList, 70, 356, 220);

    auto m_list = GJListLayer::create(list, "Join Clan", { 194, 114, 62, 255 }, 358.f, 220.f, 5);
    m_list->setPosition((dir->getWinSize().width) / 2, (dir->getWinSize().height) / 2 - 5);
    m_list->ignoreAnchorPointForPosition(false);

    auto loadingSpinner = LoadingSpinner::create(50);
    loadingSpinner->setID("loading-spinner");
    loadingSpinner->setPosition((dir->getWinSize().width) / 2, (dir->getWinSize().height) / 2);
    loadingSpinner->setVisible(false);

    addChild(bgSprite);
    addChild(menu);
    addChild(m_list);
    addChild(clanCountLabel);
    addChild(loadingSpinner);

    return true;
}

void JoinClan::keyBackClicked() {
    this->onGoBack(nullptr);
}

void JoinClan::onGoBack(CCObject*) {
    CCDirector::get()->popSceneWithTransition(0.5, PopTransition::kPopTransitionFade);
}

JoinClan* JoinClan::create() {
    auto ret = new JoinClan();
    if (ret && ret->init()) {
        ret->autorelease();
    }
    else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

CCScene* JoinClan::switchToScene() {
    auto scene = CCScene::create();
    scene->addChild(JoinClan::create());
    CCDirector::get()->pushScene(CCTransitionFade::create(.5f, scene));
    return scene;
}


void JoinClan::onCreate(cocos2d::CCObject* sender) {
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5, JoinClan::switchToScene()));
}

// Page system removed for the time being
/*void JoinClan::onPageBack(cocos2d::CCObject* sender) {
    log::info("Page back pressed!");
}

void JoinClan::onPageNext(cocos2d::CCObject* sender) {
    log::info("Page next pressed!");
}*/


void JoinClan::onRefresh(cocos2d::CCObject* sender) {

    this->getChildByID("GJListLayer")->getChildByID("list-view")->setVisible(false);
    this->getChildByID("loading-spinner")->setVisible(true);

    my_web::WebRequestTemplate reqTemplate;
    reqTemplate.getSheetData(googleSheetsScript, []() {
        CCDirector::sharedDirector()->popScene();
        auto newJoinClanScene = JoinClan::switchToScene();
        CCDirector::sharedDirector()->pushScene(newJoinClanScene);
        });
}





void JoinClan::onSearch(cocos2d::CCObject* sender) {
    log::info("Search pressed!");
}

void JoinClan::onFilter(cocos2d::CCObject* sender) {
    log::info("Filter pressed!");
}

/*void JoinClan::onPageButton(cocos2d::CCObject* sender) {
    log::info("Page button pressed!");
}*/

void JoinClan::onViewClan(cocos2d::CCObject* sender) {
    auto menuItem = static_cast<CCMenuItemSpriteExtra*>(sender);
    int clanID = reinterpret_cast<intptr_t>(menuItem->getUserData());

    auto& clansArray = g_sheetData["Clans"];

    std::vector<std::string> clanInfo;
    for (size_t i = 0; i < 11; i++) {
        auto tagResult = clansArray[clanID][i].as<std::string>();
        if (tagResult) {
            clanInfo.push_back(tagResult.unwrap());
        }
    }

    /* 
    * [0] = ID
    * [1] = Tag
    * [2] = Title
    * [3] = Desc
    * [4] = Privacy
    * [5] = Badge BG Shape (int)
    * [6] = Badge BG Col
    * [7] = Badge Icon (int)
    * [8] = Badge Icon Col
    * [9] = Clan Points (int)
    * [10] = Status
    */

    log::info("Page button pressed! ClanTag: {}, desc: {}", clanID, clanInfo[3]);
}
