#include "InfoPopup.hpp"

#include "Cache.hpp"

InfoPopup* InfoPopup::create() {
    InfoPopup* ret = new InfoPopup();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool InfoPopup::init() {
    Popup::init(270, 250, "square01_001.png");

    setTitle("Staff Team");
    m_closeBtn->setVisible(false);

    MDTextArea* textArea = MDTextArea::create(Cache::getEditors(), {220, 150});

    m_mainLayer->addChild(textArea);
    textArea->setPosition(m_size / 2 + ccp(0, 13));
    textArea->setAnchorPoint({0.5f, 0.5f});

    CCMenuItemSpriteExtra* closeButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Ok"),
        this,
        menu_selector(InfoPopup::onClose)
    );
    closeButton->setPosition({m_size.width / 2, 37});

    m_buttonMenu->addChild(closeButton);

    return true;
}