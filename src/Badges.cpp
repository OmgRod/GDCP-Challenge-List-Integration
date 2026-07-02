#include <Geode/Geode.hpp>
#include <Geode/modify/ProfilePage.hpp>
#include <Geode/modify/CommentCell.hpp>

#include "Cache.hpp"
#include "Includes.hpp"

using namespace geode::prelude;

#include <alphalaneous.badgify/include/Badgify.hpp>

static bool isEditor(int accountID) {
    for (const auto& editor : Cache::getEditorsList()) {
        if (editor.accountID == accountID) {
            return true;
        }
    }
    return false;
}

static std::optional<Role> getRole(int accountID) {
    for (const auto& editor : Cache::getEditorsList()) {
        if (editor.accountID == accountID) {
            return editor.role;
        }
    }
    return std::nullopt;
}

/*
void badgePopup(CCObject* sender) {
    switch (sender->getTag()) {
        case 1: FLAlertLayer::create("GDCP Owner", "The owner of the GDCP list!\n<cr>theres only one of these...</c>", "OK")->show(); break;
        case 2: FLAlertLayer::create("GDCP Co-Owner", "Co-Owner of GDCP!\n<cr>sadcraft suck an egg already..</c>", "OK")->show(); break;
        case 3: FLAlertLayer::create("GDCP Developer", "Developer of the list or the <cg>GDCP Geode Mod</c>!\n<cr>these ones pull the strings..</c>", "OK")->show(); break;
        case 4: FLAlertLayer::create("GDCP Moderator", "GDCP list moderators!\n<cr>one of them really likes Congregation..</c>", "OK")->show(); break;
        default: break;
    }
}

class $modify(MyProfilePage, ProfilePage) {
    void loadPageFromUserInfo(GJUserScore* a2) {
        ProfilePage::loadPageFromUserInfo(a2);

        if (!m_mainLayer || !a2) return;
        auto layer = m_mainLayer;

        auto username_menu = typeinfo_cast<CCMenu*>(layer->getChildByIDRecursive("username-menu"));
        if (!username_menu) return;

        if (isEditor(a2->m_accountID)) {
            std::string badgeString;
            auto role = getRole(a2->m_accountID);
            int tag;

            if (role.has_value()) {
                switch (role.value()) {
                    case Role::Owner: badgeString = "GDCP_owner.png"_spr; tag = 1; break;
                    case Role::Admin: badgeString = "GDCP_coOwner.png"_spr; tag = 2; break;
                    case Role::Dev: badgeString = "GDCP_developer.png"_spr; tag = 3; break;
                    case Role::Helper: badgeString = "GDCP_moderator.png"_spr; tag = 4; break;
                    default: break;
                }

                auto badgeSprite = CCSprite::createWithSpriteFrameName(badgeString.c_str());
                auto badgeButton = CCMenuItemSpriteExtra::create(
                    badgeSprite,
                    this,
                    menu_selector(MyProfilePage::onBadgeClick)
                );
                badgeButton->setTag(tag);
                badgeButton->setID("moderator-badge"_spr);

                username_menu->addChild(badgeButton);
                username_menu->updateLayout();
            }
        }
    }

    void onBadgeClick(CCObject* sender) {
        badgePopup(sender);
    }
};

class $modify(MyCommentCell, CommentCell) {
    void loadFromComment(GJComment* p0) {
        CommentCell::loadFromComment(p0);

        if (!m_mainLayer || !p0) return;
        auto layer = m_mainLayer;

        auto username_menu = typeinfo_cast<CCMenu*>(layer->getChildByIDRecursive("username-menu"));
        if (!username_menu) return;

        if (isEditor(p0->m_accountID)) {
            std::string badgeString;
            auto role = getRole(p0->m_accountID);
            int tag;

            if (role.has_value()) {
                switch (role.value()) {
                    case Role::Owner: badgeString = "GDCP_owner.png"_spr; tag = 1; break;
                    case Role::Admin: badgeString = "GDCP_coOwner.png"_spr; tag = 2; break;
                    case Role::Dev: badgeString = "GDCP_developer.png"_spr; tag = 3; break;
                    case Role::Helper: badgeString = "GDCP_moderator.png"_spr; tag = 4; break;
                    default: break;
                }

                auto badgeSprite = CCSprite::createWithSpriteFrameName(badgeString.c_str());
                auto badgeButton = CCMenuItemSpriteExtra::create(
                    badgeSprite,
                    this,
                    menu_selector(MyCommentCell::onBadgeClick)
                );
                badgeButton->setTag(tag);
                badgeButton->setID("moderator-badge"_spr);

                username_menu->addChild(badgeButton);
                username_menu->updateLayout();
            }
        }
    }

    void onBadgeClick(CCObject* sender) {
        badgePopup(sender);
    }
};
*/

$on_mod(Loaded) {
    alpha::badgify::registerBadge(
        "challenge-progression-moderator"_spr,
        "GD Challenge Progression Moderartor",
        "GD Challenge Progression list moderators!\n\n<cr>one of them really likes Congregation...</c>",
        [] (const alpha::badgify::Badge& badge) {
            const int accountID = badge.user.data()->m_accountID;
            if (!isEditor(accountID)) return;

            CCSprite* helper = CCSprite::createWithSpriteFrameName("GDCP_moderator.png"_spr);
            helper->setTag(4);

            auto role = getRole(accountID);
            if (!role.has_value() || role.value() != Role::Helper) return;

            alpha::badgify::showBadge(badge, helper);
        }
    );
    alpha::badgify::registerBadge(
        "challenge-progression-developer"_spr,
        "GD Challenge Progression Developer",
        "Developer of the list or the <cg>GD Challenge Progression Geode mod</c>!\n\n<cr>these ones pull the strings...</c>",
        [] (const alpha::badgify::Badge& badge) {
            const int accountID = badge.user.data()->m_accountID;
            if (!isEditor(accountID)) return;

            CCSprite* developer = CCSprite::createWithSpriteFrameName("GDCP_developer.png"_spr);
            developer->setTag(3);

            auto role = getRole(accountID);
            if (!role.has_value() || role.value() != Role::Dev) return;

            alpha::badgify::showBadge(badge, developer);
        }
    );
    alpha::badgify::registerBadge(
        "challenge-progression-co-owner"_spr,
        "GD Challenge Progression Co-owner",
        "Co-Owner of the GD Challenge Progression list!\n\n<cr>sadcraft, suck an egg already...</c>",
        [] (const alpha::badgify::Badge& badge) {
            const int accountID = badge.user.data()->m_accountID;
            if (!isEditor(accountID)) return;

            CCSprite* coOwner = CCSprite::createWithSpriteFrameName("GDCP_coOwner.png"_spr);
            coOwner->setTag(2);

            auto role = getRole(accountID);
            if (!role.has_value() || role.value() != Role::Admin) return;

            alpha::badgify::showBadge(badge, coOwner);
        }
    );
    alpha::badgify::registerBadge(
        "challenge-progression-owner"_spr,
        "GD Challenge Progression Owner",
        "The owner of the GD Challenge Progression list!\n\n<cr>there's only one of these...</c>",
        [] (const alpha::badgify::Badge& badge) {
            const int accountID = badge.user.data()->m_accountID;
            if (!isEditor(accountID)) return;

            CCSprite* owner = CCSprite::createWithSpriteFrameName("GDCP_owner.png"_spr);
            owner->setTag(1);

            auto role = getRole(accountID);
            if (!role.has_value() || role.value() != Role::Owner) return;

            alpha::badgify::showBadge(badge, owner);
        }
    );
}