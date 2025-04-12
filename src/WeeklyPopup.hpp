#include "Includes.hpp"

class WeeklyCell : public LevelCell  {

public:

    void draw() override {}

    static WeeklyCell* create();

    WeeklyCell(char const* p0, float p1, float p2) : LevelCell(p0, p1, p2) {}

};


class WeeklyPopup : public geode::Popup<>, public LevelManagerDelegate {

public:

    static WeeklyPopup* create();

    void showError();

    void loadLevel();

private:

    WeeklyCell* m_cell = nullptr;

    CCNode* m_cellContainer = nullptr;

    CCLabelBMFont* m_timeLabel = nullptr;
    CCLabelBMFont* m_errorLabel = nullptr;

    LoadingCircle* m_loadingCircle = nullptr;

    CCMenuItemSpriteExtra* m_discardButton = nullptr;

    bool m_isLoading = true;
    bool m_isError = false;

    ~WeeklyPopup();

    bool setup();

    void showLoading();

    void hideLoading();

    void hideError();

    void showLevel(GJGameLevel* level);

    void onDiscard(CCObject*);

    virtual void loadLevelsFinished(cocos2d::CCArray* levels, char const*, int) override;
    virtual void loadLevelsFailed(char const*, int) override;

};