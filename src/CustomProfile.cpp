#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <geode/modify/MenuLayer.hpp>
#include <geode/modify/ProfilePage.hpp>
#include <matjson.hpp>

#include "globals.hpp"
#include "JoinClan.hpp"

#include "WebRequestHelper.hpp"

using namespace geode::prelude;

class $modify(ProfilePage) {

public:

    struct Fields {
        bool isInClan = false;
        std::string clanTag = "";
    };

    void updateUserStats() {
        auto& usersArray = g_sheetData["Users"];

        int targetRow = -1;
        for (size_t row = 0; row < usersArray.size(); ++row) {
            if (usersArray[row][5] == m_accountID) {
                targetRow = row;
                break;
            }
        }

        std::vector<std::string> userInfo;
        for (size_t i = 0; i < 6; i++) {
            if (i != 1 && i != 5 && i != 6) {
                userInfo.push_back(usersArray[targetRow][i].as<std::string>().unwrap());
            }
            else {
                userInfo.push_back(std::to_string(usersArray[targetRow][i].as<int>().unwrap()));
            }
        }

        std::string colors =
            std::to_string(m_score->m_color1) + ";" +
            std::to_string(m_score->m_color2) + ";" +
            std::to_string(m_score->m_color3);

        std::string stats =
            std::to_string(m_score->m_stars) + ";" +
            std::to_string(m_score->m_moons) + ";" +
            std::to_string(m_score->m_diamonds) + ";" +
            std::to_string(m_score->m_demons) + ";" +
            std::to_string(m_score->m_creatorPoints) + ";" +
            std::to_string(m_score->m_secretCoins) + ";" +
            std::to_string(m_score->m_userCoins);

        if (targetRow != -1) {
            ++targetRow;
            my_web::WebRequestTemplate reqTemplate;
            reqTemplate.set("c", m_usernameLabel->getString())
                .set("d", userInfo[3])
                .set("e", userInfo[4])
                .set("f", userInfo[5])
                .set("g", m_score->m_playerCube)
                .set("h", colors)
                .set("i", stats)
                .set("j", m_score->m_glowEnabled)
                .set("sheetName", "Users")
                .setEdit(true)
                .setRow(targetRow);
            reqTemplate.sendRequest(googleSheetsScript, []() {});
        }
    }


    void loadPageFromUserInfo(GJUserScore * a2) {
        ProfilePage::loadPageFromUserInfo(a2);

        // Update user data in sheet (e.g. icon)
        if (m_ownProfile && g_inClan) {
            updateUserStats();
        }

        // Check if user exists in the sheet
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

            if (_stricmp(cellValue.c_str(), a2->m_userName.c_str()) == 0) {
                m_fields->isInClan = true;
                m_fields->clanTag = usersSheet[i][3].as<std::string>().unwrap();
                break;
            }
        }

        // Add Clan Tag in top name
        if (m_fields->isInClan) {
            this->m_usernameLabel->setString(addTagToUsername(m_fields->clanTag, a2->m_userName).c_str());
            this->m_usernameLabel->setScale(getNewScale(m_fields->clanTag, this->m_usernameLabel->getScale()));
        }


        // Add Clan Menu Button
        if (m_ownProfile) {
            auto bottomMenu = dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("bottom-menu");

            auto profileButtonSprite = CCSprite::create("profile_button.png"_spr);

            auto profileButton = CCMenuItemSpriteExtra::create(
                profileButtonSprite,
                this,
                menu_selector(JoinClan::onCreate)
            );


            profileButton->setID("clan-button");

            auto settingsButton = bottomMenu->getChildByID("settings-button");

            bottomMenu->insertBefore(profileButton, settingsButton);
            bottomMenu->updateLayout();
        }
    }

    // Add clan tag in comment boxes
    virtual void loadCommentsFinished(cocos2d::CCArray* p0, char const* p1) {
        loadCommentsFinished(p0, p1);

        if (m_fields->isInClan) {
            auto listView = dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("GJCommentListLayer")->getChildByID("list-view");
            auto commentListTable = dynamic_cast<TableView*>(listView->getChildren()->objectAtIndex(0));
            auto commentContentLayer = dynamic_cast<CCContentLayer*>(commentListTable->getChildren()->objectAtIndex(0));

            auto children = commentContentLayer->getChildren();
            for (int i = 0; i < children->count(); ++i) {
                auto child = dynamic_cast<CommentCell*>(children->objectAtIndex(i));
                auto commentBoxUsername = dynamic_cast<CCLabelBMFont*>(dynamic_cast<CCLayer*>(child->getChildren()->objectAtIndex(1))->getChildByID("username-label"));

                commentBoxUsername->setString(addTagToUsername(m_fields->clanTag, commentBoxUsername->getString()).c_str());
            }
        }
    }

};




