#include "Includes.hpp"
#include "DoubleArrow.h"

#include "GDCPListLayer.hpp"
#include "InfoPopup.hpp"
#include "WeeklyPopup.hpp"
#include "Request.hpp"
#include "Cache.hpp"
#include "Utils.hpp"

GDCPListLayer::~GDCPListLayer() {
    if (m_loadingCircle) m_loadingCircle->release();
    Cache::setLayer(nullptr);
}

GDCPListLayer::GDCPListLayer(bool platformer) {
    m_isPlatformer = platformer;
}

GDCPListLayer* GDCPListLayer::create(bool platformer) {
    GDCPListLayer* ret = new GDCPListLayer(platformer);
    if (ret->init(platformer)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

void GDCPListLayer::keyBackClicked() {
    GameLevelManager::sharedState()->m_levelManagerDelegate = nullptr;
    CCDirector::get()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

bool GDCPListLayer::init(bool platformer) {
    if (platformer) m_isPlatformer = true;

    Cache::setLayer(this);

    this->setKeypadEnabled(true);
    cocos::handleTouchPriority(this, true);

    CCSprite* backgroundSprite = CCSprite::create("GJ_gradientBG.png");
    backgroundSprite->setID("background");
    
    cocos2d::CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCSize size = backgroundSprite->getContentSize();
    backgroundSprite->setScaleX(winSize.width / size.width);
    backgroundSprite->setScaleY(winSize.height / size.height);
    
    backgroundSprite->setAnchorPoint({0, 0});
    backgroundSprite->setColor({100, 100, 100});
    backgroundSprite->setZOrder(-1);
    addChild(backgroundSprite); 

    CCMenu* menu = CCMenu::create();
    menu->setPosition({0, 0});
    menu->setAnchorPoint({ 0.f, 0.f });
    addChild(menu);

    CCSprite* spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(
        spr,
        this,
        menu_selector(GDCPListLayer::onBack)
    );
    button->setPosition({25, winSize.height - 25});

    menu->addChild(button);

    CCSprite* bottomLeft = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    bottomLeft->setPosition({-1,-1});
    bottomLeft->setAnchorPoint({0,0});
    addChild(bottomLeft, -1);

    CCSprite* bottomRight = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    bottomRight->setPosition({winSize.width + 1, -1});
    bottomRight->setAnchorPoint({1,0});
    bottomRight->setFlipX(true);
    addChild(bottomRight, -1);
    
    m_loadingCircle = LoadingCircle::create();
    m_loadingCircle->setParentLayer(this);
    m_loadingCircle->retain();
    m_loadingCircle->show();

    m_errorLabel = CCLabelBMFont::create("Unable to load page", "bigFont.fnt");
    m_errorLabel->setPosition(winSize / 2);
    m_errorLabel->setVisible(false);
    m_errorLabel->setOpacity(125);
    m_errorLabel->setScale(0.73f);
    addChild(m_errorLabel, 2);

    spr = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    m_refreshButton = CCMenuItemSpriteExtra::create(spr,
        this,
        menu_selector(GDCPListLayer::onRefresh)
    );
    m_refreshButton->setCascadeOpacityEnabled(true);
    m_refreshButton->setPosition({winSize.width - 27, 27});

    menu->addChild(m_refreshButton);

    spr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    spr->setFlipX(true);

    m_nextButton = CCMenuItemSpriteExtra::create(
        spr,
        this,
        menu_selector(GDCPListLayer::onNext)
    );
    m_nextButton->setCascadeOpacityEnabled(true);
    m_nextButton->setPosition({winSize.width - 24.f, winSize.height / 2.f});

    menu->addChild(m_nextButton);

    spr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");

    m_prevButton = CCMenuItemSpriteExtra::create(
        spr,
        this,
        menu_selector(GDCPListLayer::onPrev)
    );
    m_prevButton->setCascadeOpacityEnabled(true);
    m_prevButton->setPosition({24.f, winSize.height / 2.f});

    menu->addChild(m_prevButton);
    
    std::string editors = Cache::getEditors();

    spr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");

    m_infoButton = CCMenuItemSpriteExtra::create(spr, this, menu_selector(GDCPListLayer::onInfo));
    m_infoButton->setPosition({ 30.0f, 30.0f });
    m_infoButton->setCascadeOpacityEnabled(true);
    menu->addChild(m_infoButton);

    m_pageLabel = CCLabelBMFont::create("", "goldFont.fnt");
    m_pageLabel->setPosition({winSize.width - 2.f, winSize.height - 2.f});
    m_pageLabel->setAnchorPoint({1.f, 1.f});
    m_pageLabel->setScale(0.56f);
    addChild(m_pageLabel);

    if (editors.empty()) m_infoButton->setVisible(false);

    m_list = GJListLayer::create(nullptr, "GDCP Challenge List", {191, 114, 62, 255}, 356.f, 220.f, 0);
    m_list->setPosition(winSize / 2 - m_list->getScaledContentSize() / 2 - CCPoint(0,5));
    addChild(m_list);

    spr = CCSprite::create("GJ_button_02.png");
    spr->setScale(0.7f);

    m_pageButtonLabel = CCLabelBMFont::create("1", "bigFont.fnt");
    m_pageButtonLabel->setPosition(spr->getContentSize() / 2);
    spr->addChild(m_pageButtonLabel);

    CCMenuItemSpriteExtra* goToPageButton = CCMenuItemSpriteExtra::create(
        spr,
        this,
        menu_selector(GDCPListLayer::onGoToPage)
    );
    goToPageButton->setPosition({winSize.width - 21, winSize.height - 41});

    menu->addChild(goToPageButton);

    CCMenuItemSpriteExtra* lastPageButton = CCMenuItemSpriteExtra::create(
        DoubleArrow::create(true, "GJ_arrow_03_001.png"),
        this,
        menu_selector(GDCPListLayer::onLastPage)
    );
    lastPageButton->setPosition({winSize.width - 21, winSize.height - 74});

    menu->addChild(lastPageButton);

    CCMenuItemSpriteExtra* firstPageButton = CCMenuItemSpriteExtra::create(
        DoubleArrow::create(false, "GJ_arrow_03_001.png"),
        this,
        menu_selector(GDCPListLayer::onFirstPage)
    );
    firstPageButton->setPosition({21.f, winSize.height - 74});

    menu->addChild(firstPageButton);

    auto buttonIcon = CircleButtonSprite::create(
        CCSprite::createWithSpriteFrameName("gj_dailyCrown_001.png"),
        CircleBaseColor::Green,
        CircleBaseSize::Medium
    );
    buttonIcon->setScale(1.175f);

    CCMenuItemSpriteExtra* weeklyButton = CCMenuItemSpriteExtra::create(buttonIcon, this, menu_selector(GDCPListLayer::onWeekly));
    weeklyButton->setPosition({winSize.width - (weeklyButton->getContentWidth() / 2) - 10, winSize.height - 225});
    
    if (!m_isPlatformer) {
        menu->addChild(weeklyButton);
    }
    
    if (platformer) {
        CCMenuItemSpriteExtra* classicButton = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::create(
                CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png"),
                CircleBaseColor::Green,
                CircleBaseSize::Small
            ),
            this,
            menu_selector(GDCPListLayer::onClassic)
        );
        classicButton->setPosition({36, winSize.height - 225});
    
        menu->addChild(classicButton);
    } else {
        CCMenuItemSpriteExtra* platformerButton = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::create(
                CCSprite::createWithSpriteFrameName("GJ_moonsIcon_001.png"),
                CircleBaseColor::Green,
                CircleBaseSize::Small
            ),
            this,
            menu_selector(GDCPListLayer::onPlatformer)
        );
        platformerButton->setPosition({36, winSize.height - 225});
    
        menu->addChild(platformerButton);
    }

    showLoading();

    goToPage(m_currentPage);
    
    updatePageLabels();

    return true;
}

int GDCPListLayer::getLastPage() {
    if (m_isPlatformer) {
        return (Cache::getLevelCountPlat() + levelsPerPage) / levelsPerPage;
    } else {
        return (Cache::getLevelCount() + levelsPerPage) / levelsPerPage;
    }
}

void GDCPListLayer::onBack(CCObject*) {
    keyBackClicked();
}

void GDCPListLayer::onInfo(CCObject*) {
    InfoPopup::create()->show();
}

void GDCPListLayer::onWeekly(CCObject*) {
    WeeklyPopup::create()->show();
}

void GDCPListLayer::onRefresh(CCObject*) {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_lastRefresh).count();

    if (elapsed < (3 + (std::rand() % 3)) && !m_isError && m_didRefresh) return;

    m_lastRefresh = currentTime;
    m_didRefresh = true;

    Cache::clearAllCache();

    Request::loadEditors(true);
    Request::loadWeekly();

    goToPage(m_currentPage);

    hideError();
    showLoading();
    updateButtons();
}

void GDCPListLayer::goToPage(int page) {
    if (m_isPlatformer) {
        if (CCArray* cachedPage = Cache::getCachedPagePlat(page))
            showPage(cachedPage);
        else
            Request::loadPagePlat(page);
    } else {
        if (CCArray* cachedPage = Cache::getCachedPage(page))
            showPage(cachedPage);
        else
            Request::loadPage(page);
    }
}

void GDCPListLayer::showPage(cocos2d::CCArray* levels) {
    auto levelsArray = typeinfo_cast<CCArray*>(levels);
    if (levelsArray && levelsArray->count() > 0) {
        m_customListView = CustomListView::create(levels, BoomListType::Level, 220.0, 356.0);
        m_list->addChild(m_customListView);

        for (LevelCell* cell : CCArrayExt<LevelCell*>(m_customListView->m_tableView->m_cellArray)) {
            if (!typeinfo_cast<LevelCell*>(cell)) continue;

            int top = 0;
            if (m_isPlatformer) {
                top = abs(Utils::getTopForLevelIdPlat(cell->m_level->m_levelID.value()));
            } else {
                top = abs(Utils::getTopForLevelId(cell->m_level->m_levelID.value()));
            }
            std::string topStr = std::to_string(top);

            if (top == 0) topStr = "NA";
	        if (top > 150) topStr = "Legacy";

            int coins = cell->m_level->m_coins;

            CCLabelBMFont* topLabel = CCLabelBMFont::create(topStr.c_str(), top < 6 ? "goldFont.fnt" : "bigFont.fnt");
            topLabel->setOpacity(150);
            if (auto diffContainer = cell->m_mainLayer->getChildByID("difficulty-container")) {
                topLabel->setPosition({ diffContainer->getPositionX(), coins > 0 ? 1.f : 1.f });
            } else {
                log::debug("difficulty-container not found. :(");
            }
            if (Loader::get()->isModLoaded("cvolton.compact_lists") && cell->getContentSize().height < 90) {
                float scale = (topStr == "Legacy") ? 0.5f : (top < 6 ? 0.45f : 0.32f);
                topLabel->limitLabelWidth(25.f, scale, 0.001f);
                topLabel->setAnchorPoint({ 0.5f, 0.f });
                if (topStr == "Legacy") {
                    topLabel->setScale(0.25f);
                } else {
                    topLabel->setScale(scale);
                }
            } else {
                float scale = (topStr == "Legacy") ? 0.6f : (coins > 0 ? (top < 6 ? 0.55f : 0.4f) : (top < 6 ? 0.65f : 0.5f));
                topLabel->limitLabelWidth(25.f, scale, 0.001f);
                topLabel->setPositionY(coins > 0 ? 9.f : 14.f);
                if (topStr == "Legacy") {
                    topLabel->setScale(0.35f);
                } else {
                    topLabel->setScale(scale);
                }
            }

            if (top > 75) topLabel->setColor(ccc3(233, 233, 233));
            else if (top > 150) topLabel->setColor(ccc3(188, 188, 188));

            if (CCNode* mainLayer = cell->getChildByID("main-layer")) {
                mainLayer->addChild(topLabel);

                if (coins < 1) continue;

                if (CCNode* container = mainLayer->getChildByID("difficulty-container")) {
                    if (CCNode* coin1 = container->getChildByID("coin-icon-1")) {
                        coin1->setScale(0.65f);
                        coin1->setPosition(coin1->getPosition() + ccp(coins == 2 ? 1.5f : (coins == 3 ? 3.f : 0.f), 3));
                    }
                    if (CCNode* coin2 = container->getChildByID("coin-icon-2")) {
                        coin2->setScale(0.65f);
                        coin2->setPosition(coin2->getPosition() + ccp(coins == 2 ? -1.5f : 0.f, 3));
                    }
                    if (CCNode* coin3 = container->getChildByID("coin-icon-3")) {
                        coin3->setScale(0.65f);
                        coin3->setPosition(coin3->getPosition() + ccp(coins == 3 ? -3.f : 0.f, 3));
                    }
                }
            }
        }

        hideLoading();
        hideError();
        updateButtons();
    } else {
        return showError();
    }
}

void GDCPListLayer::loadPage(const std::string& str) {
    if (str.empty())
        return showError();


    GameLevelManager* glm = GameLevelManager::sharedState();
    glm->m_levelManagerDelegate = this;
    glm->getOnlineLevels(GJSearchObject::create(SearchType::Type19, str));
}

void GDCPListLayer::loadLevelsFinished(cocos2d::CCArray* levels, char const*, int) {
    if (m_isPlatformer) {
        Cache::setCachedPagePlat(m_currentPage, levels);
        showPage(levels);
    } else {
        Cache::setCachedPage(m_currentPage, levels);
        showPage(levels);
    }
}

void GDCPListLayer::loadLevelsFailed(char const*, int) {
    showError();
}

void GDCPListLayer::onNext(CCObject*) {
    m_currentPage++;

    showLoading();

    goToPage(m_currentPage);

    updateButtons();
    updatePageLabels();
}

void GDCPListLayer::onPrev(CCObject*) {
    m_currentPage--;

    if (m_currentPage < 0) m_currentPage = 0;

    showLoading();

    goToPage(m_currentPage);

    updateButtons();
    updatePageLabels();
}

void GDCPListLayer::updatePageLabels() {
    if (m_isPlatformer) {
        int pageMax = m_currentPage * levelsPerPage + levelsPerPage;
        m_pageLabel->setString(fmt::format(
            "{} to {} of {}",
            m_currentPage * levelsPerPage + 1,
            pageMax > Cache::getLevelCountPlat() ? Cache::getLevelCountPlat() : pageMax,
            Cache::getLevelCountPlat()
        ).c_str());

        m_pageButtonLabel->setString(std::to_string(m_currentPage + 1).c_str());
        m_pageButtonLabel->limitLabelWidth(28.f, 0.8f, 0.001f);
    } else {
        int pageMax = m_currentPage * levelsPerPage + levelsPerPage;
        m_pageLabel->setString(fmt::format(
            "{} to {} of {}",
            m_currentPage * levelsPerPage + 1,
            pageMax > Cache::getLevelCount() ? Cache::getLevelCount() : pageMax,
            Cache::getLevelCount()
        ).c_str());

        m_pageButtonLabel->setString(std::to_string(m_currentPage + 1).c_str());
        m_pageButtonLabel->limitLabelWidth(28.f, 0.8f, 0.001f);
    }
}

void GDCPListLayer::updateButtons() {
    bool enableArrows = !m_isLoading;

    m_nextButton->setEnabled(enableArrows && m_currentPage != getLastPage() - 1);
    m_nextButton->setOpacity(enableArrows && m_currentPage != getLastPage() - 1 ? 255 : 125);

    m_prevButton->setEnabled(enableArrows && m_currentPage != 0);
    m_prevButton->setOpacity(enableArrows && m_currentPage != 0 ? 255 : 125);

    m_refreshButton->setEnabled(!m_isLoading);
    m_refreshButton->setOpacity(!m_isLoading ? 255 : 200);

    std::string editors = Cache::getEditors();
    m_infoButton->setEnabled(!editors.empty());
    m_infoButton->setOpacity(!editors.empty() ? 255 : 125);
}

void GDCPListLayer::showError() {
    m_errorLabel->setVisible(true);
    m_isError = true;

    hideLoading();
    updateButtons();
}

void GDCPListLayer::hideError() {
    m_errorLabel->setVisible(false);
    m_isError = false;

    updateButtons();
}

void GDCPListLayer::showLoading() {
    if (m_customListView) {
        m_customListView->removeFromParentAndCleanup(true);
        m_customListView = nullptr;
    }

    m_loadingCircle->setVisible(true);
    m_isLoading = true;

    hideError();
    updateButtons();
}

void GDCPListLayer::hideLoading() {
    m_loadingCircle->setVisible(false);
    m_isLoading = false;

    updateButtons();
}

void GDCPListLayer::onGoToPage(CCObject* sender) {
    auto popup = SetIDPopup::create(m_currentPage + 1, 1, getLastPage(), "Go to Page", "Search", true, 0, 0.f, true, true);
    popup->m_delegate = this;
    
    popup->show();
}

void GDCPListLayer::setIDPopupClosed(SetIDPopup* popup, int id) {
    id = std::clamp(id, 1, getLastPage());

    m_currentPage = id - 1;

    showLoading();
    goToPage(m_currentPage);

    updateButtons();
    updatePageLabels();
}

void GDCPListLayer::onLastPage(CCObject* sender) {
    showLoading();
    m_currentPage = getLastPage() - 1;
    goToPage(m_currentPage);
    
    updateButtons();
    updatePageLabels();
}

void GDCPListLayer::onFirstPage(CCObject* sender) {
    showLoading();
    m_currentPage = 0;
    goToPage(m_currentPage);
    
    updateButtons();
    updatePageLabels();
}

void GDCPListLayer::onPlatformer(CCObject* sender) {
    auto layer = GDCPListLayer::create(true);
	auto scene = CCScene::create();
	scene->addChild(layer);

	auto transition = CCTransitionFade::create(0.5f, scene);
	
	CCDirector::sharedDirector()->replaceScene(transition);
}

void GDCPListLayer::onClassic(CCObject* sender) {
    auto layer = GDCPListLayer::create(false);
	auto scene = CCScene::create();
	scene->addChild(layer);

	auto transition = CCTransitionFade::create(0.5f, scene);
	
	CCDirector::sharedDirector()->replaceScene(transition);
}
