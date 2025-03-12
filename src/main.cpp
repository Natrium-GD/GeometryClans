#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <geode/modify/MenuLayer.hpp>
#include <geode/modify/ProfilePage.hpp>
#include <matjson.hpp>

#include "globals.hpp"
#include "WebRequestHelper.hpp"
my_web::Fields my_web::WebRequestTemplate::fields;

// Temp
#include "CreateClan.hpp"
#include "IconCreator.hpp"


using namespace geode::prelude;


// Declarations for globals.h
bool g_dataFetched = false;
matjson::Value g_sheetData = matjson::Value::array();
std::string googleSheetsScript = "https://script.google.com/macros/s/AKfycbw4BegOLKOTLu7vCIPKOu0QQc5ftLPqLnD0IYEQEmorESYlykhz4urrOThBruw-wwoshA/exec";

bool g_inClan = false;
std::string g_clanTag = "";
std::string g_username = "";


std::string addTagToUsername(const std::string& clanTag, const std::string& username) {
    std::string formattedString = "[" + clanTag + "] " + username;
    return formattedString;
}

float getNewScale(const std::string& clanTag, const float ogScale) {
    float mult = clanTag.length() * .1f;
    float newScale = ogScale * (1 - mult);
    return newScale;
}


class $modify(MenuLayer) {
    struct Fields {
        EventListener<web::WebTask> m_listener;
    };

    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        // TEMP FOR TESTING!!

        auto profileButtonSprite = CCSprite::create("profile_button.png"_spr);

        auto profileButton = CCMenuItemSpriteExtra::create(
            profileButtonSprite,
            this,
            menu_selector(CreateClan::onProfileButton)
        );

        this->getChildByID("bottom-menu")->addChild(profileButton);

        IconCreator iconCreator;
        auto iconSprite = iconCreator.buildIcon(194, "33;92;11", true);

        iconSprite->setPosition(CCPoint(280, 160));
        addChild(iconSprite);

        // TEMP END

        if (!g_dataFetched) {
            my_web::WebRequestTemplate reqTemplate;
            reqTemplate.getSheetData(googleSheetsScript, [this]() {
                g_dataFetched = true;
                g_username = std::string(this->m_profileLabel->getString());

                if (g_sheetData.contains("Users")) {
                    auto& usersSheet = g_sheetData["Users"];

                    for (size_t i = 0; i < usersSheet.size(); i++) {
                        if (!usersSheet[i].isArray() || usersSheet[i].size() <= 1) {
                            continue;
                        }

                        auto cellResult = usersSheet[i][2].as<std::string>();

                        if (!cellResult) {
                            log::error("Failed to read cell in row {}", i + 1);
                            continue;
                        }

                        std::string cellValue = cellResult.unwrap();

                        if (_stricmp(cellValue.c_str(), g_username.c_str()) == 0) {
                            g_inClan = true;
                            g_clanTag = usersSheet[i][3].as<std::string>().unwrap();

                            this->m_profileLabel->setString(addTagToUsername(g_clanTag, g_username).c_str());
                            this->m_profileLabel->setScale(getNewScale(g_clanTag, this->m_profileLabel->getScale()));
                            this->updateLayout();
                            break;
                        }
                    }
                }
            });
        }


        // Change profile label to include tag
        if (g_inClan) {
            this->m_profileLabel->setString(addTagToUsername(g_clanTag, g_username).c_str());
            this->m_profileLabel->setScale(getNewScale(g_clanTag, this->m_profileLabel->getScale()));
            this->updateLayout();
        }

        return true;

    }
};




