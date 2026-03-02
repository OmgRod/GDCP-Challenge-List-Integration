#pragma once

#include "Includes.hpp"
#include "GDCPListLayer.hpp"

class Request {

private:

    static async::TaskHolder<web::WebResponse> m_listener;

    static std::string getLevelsString(int page);
    static std::string getLevelsStringPlat(int page);

    static void loadPageLevels(int page);
    static void loadPageLevelsPlat(int page);

public:

    static void loadLevelNames(bool shouldLoadLevels = false, int page = 0);
    static void loadLevelNamesPlat(bool shouldLoadLevels = false, int page = 0);

    static void loadPage(int page);
    static void loadPagePlat(int page);

    static void loadEditors(bool shouldUpdateButtons = false);

    static void loadWeekly(bool shouldUpdate = false);
};
