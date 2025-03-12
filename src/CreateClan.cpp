#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <matjson.hpp>

#include "globals.hpp"
#include "CreateClan.hpp" 

#include "WebRequestHelper.hpp"

using namespace geode::prelude;

std::string createClanTag = "";
std::string createClanTitle = "";
std::string createClanDesc = "";
std::string createClanPrivacy = "Public";

cocos2d::ccColor3B color3BBox1 = cocos2d::ccColor3B(255, 255, 255);
cocos2d::ccColor3B color3BBox2 = cocos2d::ccColor3B(255, 255, 255);

struct Fields {
    ColorPickPopup* colorPicker = nullptr;
    int badgeBG = 1;
    int badgeIcon = 1;
};

Fields fields;

CCLabelBMFont* createLabel(const std::string& text, const std::string& font, float scale, const CCPoint& position) {
    auto label = CCLabelBMFont::create(text.c_str(), font.c_str());
    label->setScale(scale);
    label->setPosition(position);
    return label;
}

TextInput* createTextInput(float width, const std::string& initialText, const std::string& font, const CCPoint& position, int maxCharCount, const std::string& id)
{
    auto input = TextInput::create(width, initialText, font.c_str());
    input->setPosition(position);
    input->setMaxCharCount(maxCharCount);
    input->setID(id);
    return input;
}

void setInputCallback(TextInput* inputField, std::string& targetString, size_t maxLength) {
    inputField->setCallback([inputField, &targetString, maxLength](std::string p0) {
        if (p0.length() > maxLength) {
            p0 = p0.substr(0, maxLength);
        }
        targetString = inputField->getString();
        });
}



// Main Menu
void CreateClan::onProfileButton(cocos2d::CCObject* sender) {
    class CreateClanPopup : public geode::Popup<std::string const&> {
    protected:
        bool setup(std::string const& value) override {
            this->setTitle("Create Clan");

            auto menu = CCMenu::create(nullptr);
            menu->setID("CreateClanMenu");
            menu->setPosition(-10, 0);
            m_mainLayer->addChild(menu);

            auto createClanSprite = ButtonSprite::create("Create");
            auto createClanButton = CCMenuItemSpriteExtra::create(createClanSprite, this, menu_selector(CreateClan::onCreateClan));
            createClanButton->setPosition(175.f, 32.f);

            auto badgeSprite = CCSprite::create("badge_bg_1.png"_spr);
            badgeSprite->setID("previewBadgeBG");

            auto badgeSpriteHigh = CCSprite::create("badge_bg_1_highlight.png"_spr);
            badgeSpriteHigh->setID("previewBadgeBGHigh");
            badgeSpriteHigh->setPosition(CCPoint(25, 25));
            badgeSpriteHigh->setScale(0.8f);

            auto badgeSpriteExtra = CCSprite::create("badge_icon_1.png"_spr);
            badgeSpriteExtra->setID("previewBadgeExtra");
            badgeSpriteExtra->setPosition(CCPoint(25, 25));
            badgeSpriteExtra->setScale(0.8f);

            auto badgeSpriteExtraHigh = CCSprite::create("badge_icon_1_highlight.png"_spr);
            badgeSpriteExtraHigh->setID("previewBadgeExtraHigh");
            badgeSpriteExtraHigh->setPosition(CCPoint(25, 25));
            badgeSpriteExtraHigh->setScale(0.8f);

            auto badgeButton = CCMenuItemSpriteExtra::create(
                badgeSprite,
                this,
                menu_selector(CreateClan::onEditBadge)
            );
            badgeButton->addChild(badgeSpriteHigh);
            badgeButton->addChild(badgeSpriteExtra);
            badgeButton->addChild(badgeSpriteExtraHigh);

            badgeSprite->setScale(0.8f);
            badgeButton->setPosition(55.f, 172.f);
            badgeButton->setID("previewBadge");

            auto tagLabel = createLabel("Tag", "bigFont.fnt", 0.35f, CCPoint(280.f, 195.f));
            tagLabel->setID("tagLabel");
            auto titleLabel = createLabel("Title", "bigFont.fnt", 0.35f, CCPoint(160.f, 195.f));
            titleLabel->setID("titleLabel");
            auto descLabel = createLabel("Description", "bigFont.fnt", 0.35f, CCPoint(175.f, 145.f));
            descLabel->setID("descLabel");
            auto privacyLabel = createLabel("Join Policy", "bigFont.fnt", 0.35f, CCPoint(167.f, 95.f));

            auto tagInput = createTextInput(65.0F, "Tag", "bigFont.fnt", CCPoint(280.f, 172.f), 4, "tagInput");
            auto titleInput = createTextInput(150.0F, "Title", "bigFont.fnt", CCPoint(160.f, 172.f), 15, "titleInput");
            auto clanDesc = createTextInput(220.0F, "Add a Description [Optional]", "chatFont.fnt", CCPoint(175.f, 120.f), 75, "clanDesc");
            clanDesc->setCommonFilter(CommonFilter::Any);
            clanDesc->getBGSprite()->setContentSize({ 553.0F, 60.0F });

            auto textBoxPlaceholder = static_cast<CCLayer*>(clanDesc->getChildren()->objectAtIndex(1));
            textBoxPlaceholder->setScale(1.2f);
            textBoxPlaceholder->setAnchorPoint(CCPoint(0, 0));


            auto privPublicSprite = ButtonSprite::create("Public", 90, true, "goldFont.fnt", "GJ_button_02.png", 30.f, 0.8f);
            auto privPublicButton = CCMenuItemSpriteExtra::create(privPublicSprite, this, menu_selector(CreateClan::onPrivPublic));
            privPublicButton->setID("privPublicButton");
            privPublicButton->setPosition(79.f, 73.f);
            privPublicSprite->setScale(0.8f);
            privPublicSprite->setID("privPublicSprite");

            auto privInviteSprite = ButtonSprite::create("Invite", 90, true, "goldFont.fnt", "GJ_button_05.png", 30.f, 0.8f);
            auto privInviteButton = CCMenuItemSpriteExtra::create(privInviteSprite, this, menu_selector(CreateClan::onPrivInvite));
            privInviteButton->setID("privInviteButton");
            privInviteButton->setPosition(273.f, 73.f);
            privInviteSprite->setScale(0.8f);
            privInviteSprite->setID("privInviteSprite");

            auto privRequestSprite = ButtonSprite::create("Request", 90, true, "goldFont.fnt", "GJ_button_05.png", 30.f, 0.8f);
            auto privRequestButton = CCMenuItemSpriteExtra::create(privRequestSprite, this, menu_selector(CreateClan::onPrivRequest));
            privRequestButton->setID("privRequestButton");
            privRequestButton->setPosition(175.f, 73.f);
            privRequestSprite->setScale(0.8f);
            privRequestSprite->setID("privRequestSprite");

            auto spriteInfo = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
            auto btnInfo = CCMenuItemSpriteExtra::create(spriteInfo, this, menu_selector(CreateClan::onInfoButton));
            spriteInfo->setScale(0.55f);
            btnInfo->setPosition(211.f, 94.f);

            auto clanGuidelinesSprite = CCSprite::createWithSpriteFrameName("GJ_reportBtn_001.png");
            clanGuidelinesSprite->setScale(0.7);
            auto clanGuidelinesButton = CCMenuItemSpriteExtra::create(clanGuidelinesSprite, this, menu_selector(CreateClan::onClanGuidelines));
            clanGuidelinesButton->setPosition(315.f, 25.f);

            menu->addChild(createClanButton);
            menu->addChild(badgeButton);

            menu->addChild(tagLabel);
            menu->addChild(tagInput);

            menu->addChild(titleLabel);
            menu->addChild(titleInput);

            menu->addChild(descLabel);
            menu->addChild(clanDesc);

            menu->addChild(privacyLabel);
            menu->addChild(privPublicButton);
            menu->addChild(privInviteButton);
            menu->addChild(privRequestButton);

            menu->addChild(btnInfo);
            menu->addChild(clanGuidelinesButton);

            return true;
        }

    public:
        static CreateClanPopup* create() {
            auto ret = new CreateClanPopup();
            if (ret->initAnchored(330.f, 240.f, "", "GJ_square04.png")) {
                ret->autorelease();
                return ret;
            }

            delete ret;
            return nullptr;
        }
    };

    auto popup = CreateClanPopup::create();
    if (popup) {
        popup->show();
        popup->setID("ClanPopup");

        auto popupMenu = dynamic_cast<CCLayer*>(popup->getChildren()->objectAtIndex(0))->getChildByID("CreateClanMenu");
        auto titleInput = dynamic_cast<TextInput*>(popupMenu->getChildByID("titleInput"));
        auto tagInput = dynamic_cast<TextInput*>(popupMenu->getChildByID("tagInput"));
        auto descInput = dynamic_cast<TextInput*>(popupMenu->getChildByID("clanDesc"));

        setInputCallback(titleInput, createClanTitle, 15);
        setInputCallback(tagInput, createClanTag, 4);
        setInputCallback(descInput, createClanDesc, 75);
    }
};


void CreateClan::onCreateClan(CCObject* sender) {
    auto createClanMenu = dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("CreateClanMenu");


    if (createClanTag.length() < 2 || createClanTag.length() > 4) {
        auto tagLabel = dynamic_cast<CCLabelBMFont*>(createClanMenu->getChildByID("tagLabel"));
        tagLabel->setColor(cocos2d::ccColor3B(255, 0, 0));
        tagLabel->setString("2-4 char.");
        return;
    }
    else {
        auto tagLabel = dynamic_cast<CCLabelBMFont*>(createClanMenu->getChildByID("tagLabel"));
        tagLabel->setColor(cocos2d::ccColor3B(255, 255, 255));
        tagLabel->setString("Tag");
    }

    if (createClanTitle.length() < 1 || createClanTitle.length() > 15) {
        auto titleLabel = dynamic_cast<CCLabelBMFont*>(createClanMenu->getChildByID("titleLabel"));
        titleLabel->setColor(cocos2d::ccColor3B(255, 0, 0));
        titleLabel->setString("1-15 char.");
        return;
    }
    else {
        auto titleLabel = dynamic_cast<CCLabelBMFont*>(createClanMenu->getChildByID("titleLabel"));
        titleLabel->setColor(cocos2d::ccColor3B(255, 255, 255));
        titleLabel->setString("Title");
    }

    if (createClanDesc.length() > 75) {
        auto descLabel = dynamic_cast<CCLabelBMFont*>(createClanMenu->getChildByID("descLabel"));
        descLabel->setColor(cocos2d::ccColor3B(255, 0, 0));
        descLabel->setString("Below 75 char.");
        return;
    }
    else {
        auto descLabel = dynamic_cast<CCLabelBMFont*>(createClanMenu->getChildByID("descLabel"));
        descLabel->setColor(cocos2d::ccColor3B(255, 255, 255));
        descLabel->setString("Description");
    }

    this->removeFromParent();

    my_web::WebRequestTemplate sendClanInfo;
    sendClanInfo.set("c", createClanTag)
        .set("d", createClanTitle)
        .set("e", createClanDesc)
        .set("f", createClanPrivacy)
        .set("g", fields.badgeBG)
        .set("h", color3BBox1)
        .set("i", fields.badgeIcon)
        .set("j", color3BBox2)
        .set("k", 0)
        .set("l", "Active")
        .set("sheetName", "Clans");

    sendClanInfo.sendRequest(googleSheetsScript, [&]() {
        my_web::WebRequestTemplate sendUserInfo;
        sendUserInfo.set("c", g_username)
            .set("d", createClanTitle)
            .set("e", "Owner")
            .set("sheetName", "Users");

        sendUserInfo.sendRequest(googleSheetsScript, []() {});
        });
};

void CreateClan::onPrivPublic(CCObject* sender) {
    auto privPublicButton = dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("CreateClanMenu")->getChildByID("privPublicButton");
    auto privInviteButton = dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("CreateClanMenu")->getChildByID("privInviteButton");
    auto privRequestButton = dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("CreateClanMenu")->getChildByID("privRequestButton");

    auto blue = CCTextureCache::get()->textureForKey("GJ_button_02.png");
    auto gray = CCTextureCache::get()->textureForKey("GJ_button_05.png");

    auto privPublicSprite = dynamic_cast<CCSpriteBatchNode*>(dynamic_cast<CCScale9Sprite*>(privPublicButton->getChildByID("privPublicSprite")->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0));
    privPublicSprite->setTexture(blue);

    auto privInviteSprite = dynamic_cast<CCSpriteBatchNode*>(dynamic_cast<CCScale9Sprite*>(privInviteButton->getChildByID("privInviteSprite")->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0));
    privInviteSprite->setTexture(gray);

    auto privRequestSprite = dynamic_cast<CCSpriteBatchNode*>(dynamic_cast<CCScale9Sprite*>(privRequestButton->getChildByID("privRequestSprite")->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0));
    privRequestSprite->setTexture(gray);

    createClanPrivacy = "Public";
};

void CreateClan::onPrivInvite(CCObject* sender) {
    auto privPublicButton = dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("CreateClanMenu")->getChildByID("privPublicButton");
    auto privInviteButton = dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("CreateClanMenu")->getChildByID("privInviteButton");
    auto privRequestButton = dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("CreateClanMenu")->getChildByID("privRequestButton");

    auto blue = CCTextureCache::get()->textureForKey("GJ_button_02.png");
    auto gray = CCTextureCache::get()->textureForKey("GJ_button_05.png");

    auto privPublicSprite = dynamic_cast<CCSpriteBatchNode*>(dynamic_cast<CCScale9Sprite*>(privPublicButton->getChildByID("privPublicSprite")->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0));
    privPublicSprite->setTexture(gray);

    auto privInviteSprite = dynamic_cast<CCSpriteBatchNode*>(dynamic_cast<CCScale9Sprite*>(privInviteButton->getChildByID("privInviteSprite")->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0));
    privInviteSprite->setTexture(blue);

    auto privRequestSprite = dynamic_cast<CCSpriteBatchNode*>(dynamic_cast<CCScale9Sprite*>(privRequestButton->getChildByID("privRequestSprite")->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0));
    privRequestSprite->setTexture(gray);

    createClanPrivacy = "Invite";
};

void CreateClan::onPrivRequest(CCObject* sender) {
    auto privPublicButton = dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("CreateClanMenu")->getChildByID("privPublicButton");
    auto privInviteButton = dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("CreateClanMenu")->getChildByID("privInviteButton");
    auto privRequestButton = dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("CreateClanMenu")->getChildByID("privRequestButton");

    auto blue = CCTextureCache::get()->textureForKey("GJ_button_02.png");
    auto gray = CCTextureCache::get()->textureForKey("GJ_button_05.png");

    auto privPublicSprite = dynamic_cast<CCSpriteBatchNode*>(dynamic_cast<CCScale9Sprite*>(privPublicButton->getChildByID("privPublicSprite")->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0));
    privPublicSprite->setTexture(gray);

    auto privInviteSprite = dynamic_cast<CCSpriteBatchNode*>(dynamic_cast<CCScale9Sprite*>(privInviteButton->getChildByID("privInviteSprite")->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0));
    privInviteSprite->setTexture(gray);

    auto privRequestSprite = dynamic_cast<CCSpriteBatchNode*>(dynamic_cast<CCScale9Sprite*>(privRequestButton->getChildByID("privRequestSprite")->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0));
    privRequestSprite->setTexture(blue);

    createClanPrivacy = "Request";
};

void CreateClan::onEditBadge(CCObject* sender) {
    class ChangeBadgePopup : public geode::Popup<std::string const&> {
    protected:
        bool setup(std::string const& value) override {
            this->setTitle("Change Badge");

            auto menu = CCMenu::create(nullptr);
            menu->setID("ChangeBadgeMenu");
            menu->setPosition(-10, 0);
            m_mainLayer->addChild(menu);

            auto badgeSprite = CCSprite::create("badge_bg_1.png"_spr);
            auto badgeSpriteHighlight = CCSprite::create("badge_bg_1_highlight.png"_spr);
            auto badgeSpriteExtra = CCSprite::create("badge_icon_1.png"_spr);
            auto badgeSpriteExtraHighlight = CCSprite::create("badge_icon_1_highlight.png"_spr);

            std::vector<CCTexture2D*> badgeTextures = {
                CCTextureCache::get()->textureForKey("badge_bg_1.png"_spr),
                CCTextureCache::get()->textureForKey("badge_bg_2.png"_spr),
                CCTextureCache::get()->textureForKey("badge_bg_3.png"_spr),
                CCTextureCache::get()->textureForKey("badge_bg_4.png"_spr),
                CCTextureCache::get()->textureForKey("badge_bg_6.png"_spr)
            };

            std::vector<CCTexture2D*> badgeHighlightTextures = {
                CCTextureCache::get()->textureForKey("badge_bg_1_highlight.png"_spr),
                CCTextureCache::get()->textureForKey("badge_bg_2_highlight.png"_spr),
                CCTextureCache::get()->textureForKey("badge_bg_3_highlight.png"_spr),
                CCTextureCache::get()->textureForKey("badge_bg_4_highlight.png"_spr),
                CCTextureCache::get()->textureForKey("badge_bg_6_highlight.png"_spr)
            };

            if (fields.badgeBG >= 1 && fields.badgeBG <= 5) {
                badgeSprite->setTexture(badgeTextures[fields.badgeBG - 1]);
                badgeSpriteHighlight->setTexture(badgeHighlightTextures[fields.badgeBG - 1]);
            }

            std::vector<CCTexture2D*> previewbadgeSprites = {
                CCTextureCache::get()->textureForKey("badge_icon_1.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_2.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_3.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_4.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_5.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_6.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_7.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_8.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_9.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_10.png"_spr)
            };

            std::vector<CCTexture2D*> previewbadgeSpritesHighlights = {
                CCTextureCache::get()->textureForKey("badge_icon_1_highlight.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_2_highlight.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_3_highlight.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_4_highlight.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_5_highlight.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_6_highlight.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_7_highlight.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_8_highlight.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_9_highlight.png"_spr),
                CCTextureCache::get()->textureForKey("badge_icon_10_highlight.png"_spr)
            };

            if (fields.badgeIcon >= 1 && fields.badgeIcon <= static_cast<int>(previewbadgeSprites.size())) {
                badgeSpriteExtra->setTexture(previewbadgeSprites[fields.badgeIcon - 1]);
                badgeSpriteExtraHighlight->setTexture(previewbadgeSpritesHighlights[fields.badgeIcon - 1]);
            }

            badgeSprite->setColor(color3BBox1);
            badgeSpriteExtra->setColor(color3BBox2);

            badgeSprite->setID("badgeSprite");
            badgeSpriteHighlight->setID("badgeSpriteHighlight");
            badgeSpriteExtra->setID("badgeSpriteExtra");
            badgeSpriteExtraHighlight->setID("badgeSpriteExtraHighlight");

            std::vector<CCSprite*> previewBadge = { badgeSprite, badgeSpriteHighlight, badgeSpriteExtra, badgeSpriteExtraHighlight };
            for (auto& badge : previewBadge) {
                if (badge) {
                    badge->setScale(1.25f);
                    badge->setPosition(CCPoint(50, 120));
                    m_mainLayer->addChild(badge);
                }
            }


            auto changeBadgeSprite = ButtonSprite::create("OK");
            auto changeBadgeButton = CCMenuItemSpriteExtra::create(changeBadgeSprite, this, menu_selector(CreateClan::onBadgeChange));
            changeBadgeButton->setPosition(165.f, 30.f);
            menu->addChild(changeBadgeButton);


            // badge background
            std::vector<std::string> badgeFilenames = {
                "badge_bg_1.png"_spr, "badge_bg_2.png"_spr, "badge_bg_3.png"_spr, "badge_bg_4.png"_spr, "badge_bg_6.png"_spr
            };

            std::vector<CCSprite*> badgeSprites;

            for (const auto& filename : badgeFilenames) {
                auto badge = CCSprite::create(filename.c_str());
                if (badge) {
                    badge->setScale(0.66);
                    badgeSprites.push_back(badge);
                }
            }

            std::vector<CCMenuItemSpriteExtra*> badgeButtons;

            for (size_t i = 0; i < badgeSprites.size(); i++) {
                auto button = CCMenuItemSpriteExtra::create(badgeSprites[i], this, menu_selector(CreateClan::onBadgeBGChanged));
                if (button) {
                    button->setPosition(120 + (i * 40), 150);
                    button->setUserData(reinterpret_cast<void*>(i + 1));
                    menu->addChild(button);
                    badgeButtons.push_back(button);
                }
            }

            // badge highlight, needs to be created for the preview badge to render correctly
            std::vector<std::string> badgeHighlighFilenames = {
                "badge_bg_1_highlight.png"_spr, "badge_bg_2_highlight.png"_spr, "badge_bg_3_highlight.png"_spr, "badge_bg_4_highlight.png"_spr, "badge_bg_6_highlight.png"_spr
            };

            std::vector<CCSprite*> badgeHighlightSprites;

            for (const auto& filename : badgeHighlighFilenames) {
                auto badge = CCSprite::create(filename.c_str());
                if (badge) {
                    badgeHighlightSprites.push_back(badge);
                }
            }

            // icon layered on top of the badge
            std::vector<std::string> badgeExtraFilenames = {
                "badge_icon_1.png"_spr, "badge_icon_2.png"_spr, "badge_icon_3.png"_spr,
                "badge_icon_4.png"_spr, "badge_icon_5.png"_spr, "badge_icon_6.png"_spr,
                "badge_icon_7.png"_spr, "badge_icon_8.png"_spr, "badge_icon_9.png"_spr,
                "badge_icon_10.png"_spr
            };

            std::vector<CCSprite*> badgeExtraSprites;

            for (const auto& filename : badgeExtraFilenames) {
                auto badgeExtra = CCSprite::create(filename.c_str());
                if (badgeExtra) {
                    badgeExtraSprites.push_back(badgeExtra);
                }
            }

            std::vector<CCMenuItemSpriteExtra*> badgeExtraButtons;

            for (size_t i = 0; i < badgeExtraSprites.size(); i++) {
                auto button = CCMenuItemSpriteExtra::create(badgeExtraSprites[i], this, menu_selector(CreateClan::onBadgeExtraChanged));
                if (button) {
                    button->setPosition(120 + (i % 5) * 40, 105 - (i / 5) * 35);
                    button->setUserData(reinterpret_cast<void*>(i + 1));
                    menu->addChild(button);
                    badgeExtraButtons.push_back(button);
                }
            }

            std::vector<std::string> badgeExtraHighlighFilenames = {
                "badge_icon_1_highlight.png"_spr, "badge_icon_2_highlight.png"_spr, "badge_icon_3_highlight.png"_spr,
                "badge_icon_4_highlight.png"_spr, "badge_icon_5_highlight.png"_spr, "badge_icon_6_highlight.png"_spr,
                "badge_icon_7_highlight.png"_spr, "badge_icon_8_highlight.png"_spr, "badge_icon_9_highlight.png"_spr,
                "badge_icon_10_highlight.png"_spr
            };

            std::vector<CCSprite*> badgeExtraHighlightSprites;

            for (const auto& filename : badgeExtraHighlighFilenames) {
                auto badge = CCSprite::create(filename.c_str());
                if (badge) {
                    badgeExtraHighlightSprites.push_back(badge);
                }
            }

            auto selector = CCSprite::createWithSpriteFrameName("GJ_select_001.png");
            selector->setPosition(CCPoint((80 + 40 * fields.badgeBG), 150));
            selector->setID("selector");

            auto selector2 = CCSprite::createWithSpriteFrameName("GJ_select_001.png");
            float xPos = 80 + 40 * (fields.badgeIcon <= 5 ? fields.badgeIcon : fields.badgeIcon - 5);
            float yPos = (fields.badgeIcon <= 5) ? 105.f : 70.f;

            selector2->setPosition(CCPoint(xPos, yPos));
            selector2->setID("selector2");

            menu->addChild(selector);
            menu->addChild(selector2);

            auto colorBox1 = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
            colorBox1->setScale(0.7f);
            auto colorBoxButton1 = CCMenuItemSpriteExtra::create(colorBox1, this, menu_selector(CreateClan::onColorBox1));
            colorBoxButton1->setPosition(45, 70);
            colorBoxButton1->setColor(color3BBox1);
            colorBoxButton1->setID("colorBoxButton1");
            menu->addChild(colorBoxButton1);

            auto colorBox2 = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
            colorBox2->setScale(0.7f);
            auto colorBoxButton2 = CCMenuItemSpriteExtra::create(colorBox2, this, menu_selector(CreateClan::onColorBox2));
            colorBoxButton2->setColor(color3BBox2);
            colorBoxButton2->setPosition(75, 70);
            colorBoxButton2->setID("colorBoxButton2");
            menu->addChild(colorBoxButton2);


            return true;
        }

    public:
        static ChangeBadgePopup* create() {
            auto ret = new ChangeBadgePopup();
            if (ret->initAnchored(310.f, 210.f, "", "GJ_square04.png")) {
                ret->autorelease();
                return ret;
            }

            delete ret;
            return nullptr;
        }
    };

    auto popup = ChangeBadgePopup::create();
    if (popup) {
        popup->show();
        popup->setID("ChangeBadge");
    }
};

void CreateClan::onInfoButton(CCObject* sender) {
    auto alert = FLAlertLayer::create(
        nullptr, "Join Policy Help",
        "<cg>Public</c> allows <cl>everyone</c> to join your clan.\n<cy>Request</c> requires members to <cl>request access</c> and Admins to accept them to join your clan.\n<cr>Invite</c> requires Admins to <cl>invite members</c> to join your clan.",
        "OK", nullptr, 400.f);
    alert->show();
}

void CreateClan::onClanGuidelines(CCObject* sender) {
    auto alert = FLAlertLayer::create(
        nullptr, "Clan Guidelines",
        "<cy>The clan name, description and tag are public.</c>\nHateful, abusive, or otherwise inappropriate content will lead to a <co>warning</c> sent to the Clan Admins or the <cr>permanent deletion</c> of the clan.",
        "OK", nullptr, 375.f);
    alert->show();
}

void CreateClan::onBadgeChange(CCObject* sender) {
    auto previewBadge = dynamic_cast<CCLayer*>(this->getParent()->getChildByID("ClanPopup")->getChildren()->objectAtIndex(0))->getChildByID("CreateClanMenu")->getChildByID("previewBadge");
    auto previewBadgeBG = dynamic_cast<CCSprite*>(previewBadge->getChildByID("previewBadgeBG"));
    auto previewBadgeBGHigh = dynamic_cast<CCSprite*>(previewBadge->getChildByID("previewBadgeBGHigh"));
    auto previewBadgeExtra = dynamic_cast<CCSprite*>(previewBadge->getChildByID("previewBadgeExtra"));
    auto previewBadgeExtraHigh = dynamic_cast<CCSprite*>(previewBadge->getChildByID("previewBadgeExtraHigh"));

    previewBadgeBG->setColor(color3BBox1);
    previewBadgeExtra->setColor(color3BBox2);

    std::vector<CCTexture2D*> badgeTextures = {
    CCTextureCache::get()->textureForKey("badge_bg_1.png"_spr),
    CCTextureCache::get()->textureForKey("badge_bg_2.png"_spr),
    CCTextureCache::get()->textureForKey("badge_bg_3.png"_spr),
    CCTextureCache::get()->textureForKey("badge_bg_4.png"_spr),
    CCTextureCache::get()->textureForKey("badge_bg_6.png"_spr)
    };

    std::vector<CCTexture2D*> badgeHighlightTextures = {
        CCTextureCache::get()->textureForKey("badge_bg_1_highlight.png"_spr),
        CCTextureCache::get()->textureForKey("badge_bg_2_highlight.png"_spr),
        CCTextureCache::get()->textureForKey("badge_bg_3_highlight.png"_spr),
        CCTextureCache::get()->textureForKey("badge_bg_4_highlight.png"_spr),
        CCTextureCache::get()->textureForKey("badge_bg_6_highlight.png"_spr)
    };

    if (fields.badgeBG >= 1 && fields.badgeBG <= 5) {
        previewBadgeBG->setTexture(badgeTextures[fields.badgeBG - 1]);
        previewBadgeBGHigh->setTexture(badgeHighlightTextures[fields.badgeBG - 1]);
    }

    std::vector<CCTexture2D*> badgeSprites = {
    CCTextureCache::get()->textureForKey("badge_icon_1.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_2.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_3.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_4.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_5.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_6.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_7.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_8.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_9.png"_spr),
     CCTextureCache::get()->textureForKey("badge_icon_10.png"_spr)
    };

    std::vector<CCTexture2D*> badgeSpritesHighlights = {
    CCTextureCache::get()->textureForKey("badge_icon_1_highlight.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_2_highlight.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_3_highlight.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_4_highlight.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_5_highlight.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_6_highlight.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_7_highlight.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_8_highlight.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_9_highlight.png"_spr),
        CCTextureCache::get()->textureForKey("badge_icon_10_highlight.png"_spr)
    };

    if (fields.badgeIcon >= 1 && fields.badgeIcon <= static_cast<int>(badgeSprites.size())) {
        previewBadgeExtra->setTexture(badgeSprites[fields.badgeIcon - 1]);
        previewBadgeExtraHigh->setTexture(badgeSpritesHighlights[fields.badgeIcon - 1]);
    }
    auto buttonToRemove = this->getParent()->getChildByID("ChangeBadge");
    buttonToRemove->removeAllChildrenWithCleanup(true);
    buttonToRemove->removeFromParentAndCleanup(true);
}

void CreateClan::onBadgeBGChanged(CCObject* sender) {
    auto item = static_cast<CCMenuItemSpriteExtra*>(sender);
    int badgeID = reinterpret_cast<int>(item->getUserData());

    fields.badgeBG = badgeID;


    auto popupMenu = dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("ChangeBadgeMenu");
    auto selector = popupMenu->getChildByID("selector");
    auto badgeSprite = dynamic_cast<CCSprite*>(dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("badgeSprite"));
    auto badgeSpriteHighlight = dynamic_cast<CCSprite*>(dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("badgeSpriteHighlight"));

    selector->setPosition((80 + 40 * badgeID), 150);

    std::vector<CCTexture2D*> badgeTextures = {
        CCTextureCache::get()->textureForKey("badge_bg_1.png"_spr),
        CCTextureCache::get()->textureForKey("badge_bg_2.png"_spr),
        CCTextureCache::get()->textureForKey("badge_bg_3.png"_spr),
        CCTextureCache::get()->textureForKey("badge_bg_4.png"_spr),
        CCTextureCache::get()->textureForKey("badge_bg_6.png"_spr)
    };

    std::vector<CCTexture2D*> badgeHighlightTextures = {
        CCTextureCache::get()->textureForKey("badge_bg_1_highlight.png"_spr),
        CCTextureCache::get()->textureForKey("badge_bg_2_highlight.png"_spr),
        CCTextureCache::get()->textureForKey("badge_bg_3_highlight.png"_spr),
        CCTextureCache::get()->textureForKey("badge_bg_4_highlight.png"_spr),
        CCTextureCache::get()->textureForKey("badge_bg_6_highlight.png"_spr)
    };

    if (badgeID >= 1 && badgeID <= 5) {
        badgeSprite->setTexture(badgeTextures[badgeID - 1]);
        badgeSpriteHighlight->setTexture(badgeHighlightTextures[badgeID - 1]);
    }

}

void CreateClan::onBadgeExtraChanged(CCObject* sender) {
    auto item = static_cast<CCMenuItemSpriteExtra*>(sender);
    int badgeID = reinterpret_cast<int>(item->getUserData());

    fields.badgeIcon = badgeID;

    auto popupMenu = dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("ChangeBadgeMenu");
    auto selector = popupMenu->getChildByID("selector2");
    auto badgeSprite = dynamic_cast<CCSprite*>(dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("badgeSpriteExtra"));
    auto badgeSpriteHighlight = dynamic_cast<CCSprite*>(dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("badgeSpriteExtraHighlight"));

    float xPos = 80 + 40 * (badgeID <= 5 ? badgeID : badgeID - 5);
    float yPos = (badgeID <= 5) ? 105.f : 70.f;

    selector->setPosition(xPos, yPos);

    std::vector<CCTexture2D*> badgeSprites = {
        CCTextureCache::get()->textureForKey("badge_icon_1.png"_spr),
        CCTextureCache::get()->textureForKey("badge_icon_2.png"_spr),
        CCTextureCache::get()->textureForKey("badge_icon_3.png"_spr),
        CCTextureCache::get()->textureForKey("badge_icon_4.png"_spr),
        CCTextureCache::get()->textureForKey("badge_icon_5.png"_spr),
        CCTextureCache::get()->textureForKey("badge_icon_6.png"_spr),
        CCTextureCache::get()->textureForKey("badge_icon_7.png"_spr),
        CCTextureCache::get()->textureForKey("badge_icon_8.png"_spr),
        CCTextureCache::get()->textureForKey("badge_icon_9.png"_spr),
         CCTextureCache::get()->textureForKey("badge_icon_10.png"_spr)
    };

    std::vector<CCTexture2D*> badgeSpritesHighlights = {
    CCTextureCache::get()->textureForKey("badge_icon_1_highlight.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_2_highlight.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_3_highlight.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_4_highlight.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_5_highlight.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_6_highlight.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_7_highlight.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_8_highlight.png"_spr),
    CCTextureCache::get()->textureForKey("badge_icon_9_highlight.png"_spr),
        CCTextureCache::get()->textureForKey("badge_icon_10_highlight.png"_spr)
    };

    if (badgeID >= 1 && badgeID <= static_cast<int>(badgeSprites.size())) {
        badgeSprite->setTexture(badgeSprites[badgeID - 1]);
        badgeSpriteHighlight->setTexture(badgeSpritesHighlights[badgeID - 1]);
    }

};

class ColorPickDelegateHandler : public geode::ColorPickPopupDelegate {
public:
    ColorPickDelegateHandler(CreateClan* parent, int boxId)
        : m_parent(parent), m_boxId(boxId) {}

    void updateColor(cocos2d::ccColor4B const& color) override {
        m_parent->colorValueChanged(color, m_boxId);
    }

private:
    CreateClan* m_parent;
    int m_boxId;
};


void CreateClan::onColorBox1(CCObject* sender) {
    auto colorPicker = ColorPickPopup::create(color3BBox1);
    auto colorDelegate = new ColorPickDelegateHandler(this, 1);
    colorPicker->setDelegate(colorDelegate);
    this->addChild(colorPicker);
}

void CreateClan::onColorBox2(CCObject* sender) {
    auto colorPicker = ColorPickPopup::create(color3BBox2);
    auto colorDelegate = new ColorPickDelegateHandler(this, 2);
    colorPicker->setDelegate(colorDelegate);
    this->addChild(colorPicker);
}

void CreateClan::colorValueChanged(cocos2d::ccColor4B const& color, int boxId) {
    cocos2d::ccColor3B color3B = cocos2d::ccColor3B(color.r, color.g, color.b);

    auto popupMenu = dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("ChangeBadgeMenu");

    if (boxId == 1) {
        color3BBox1 = color3B;
        auto colorBoxButton1 = dynamic_cast<CCSprite*>(popupMenu->getChildByID("colorBoxButton1")->getChildren()->objectAtIndex(0));
        if (colorBoxButton1) {
            colorBoxButton1->setColor(color3B);
        }

        auto badgeSprite = dynamic_cast<CCSprite*>(dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("badgeSprite"));
        if (badgeSprite) {
            badgeSprite->setColor(color3B);
        }
    }
    else if (boxId == 2) {
        color3BBox2 = color3B;
        auto colorBoxButton2 = dynamic_cast<CCSprite*>(popupMenu->getChildByID("colorBoxButton2")->getChildren()->objectAtIndex(0));
        if (colorBoxButton2) {
            colorBoxButton2->setColor(color3B);
        }

        auto badgeExtraSprite = dynamic_cast<CCSprite*>(dynamic_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildByID("badgeSpriteExtra"));
        if (badgeExtraSprite) {
            badgeExtraSprite->setColor(color3B);
        }
    }
}