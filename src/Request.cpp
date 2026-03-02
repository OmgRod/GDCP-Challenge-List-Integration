#include <Geode/utils/web.hpp>

#include "Cache.hpp"
#include "Request.hpp"
#include "Utils.hpp"
#include "WeeklyPopup.hpp"

using namespace geode::prelude;

/// @brief Classic Levels

void Request::loadPage(int page) {
    if (Cache::getLevelNames().empty())
        return loadLevelNames(true, page);

    loadPageLevels(page);
}

void Request::loadLevelNames(bool shouldLoadLevels, int page) {

    if (!Cache::getLevelNames().empty() && shouldLoadLevels)
        return loadPageLevels(page);

    auto req = web::WebRequest();

    req.header("Content-Type", "application/json");

    auto listener = std::make_shared<async::TaskHolder<web::WebResponse>>();

    listener->spawn(
        req.get(fmt::format("{}{}.json", listLink, "_list")),
        [listener, shouldLoadLevels, page](web::WebResponse value) {
            auto res = value.json();
            GDCPListLayer* layer = Utils::getLayer();

            if (res.isErr()) {
                if (layer) layer->showError();
                
                return log::error("1. Failed to load level names: {}", res.unwrapErr());
            }

            auto json = res.unwrap().asArray();

            if (json.isErr()) {
                if (layer) layer->showError();
                
                return log::error("2. Failed to load level names: {}", json.unwrapErr());
            }

            std::vector<std::string> names;
            std::vector<std::string> realnames;

            for (auto& value : json.unwrap()) {
                std::string name = value.asString().unwrapOr("");
                if (name.empty()) continue;
                names.push_back(name);
            }

            Cache::setLevelNames(names);

            if (shouldLoadLevels)
                loadPageLevels(page);
            }
    );
}

void Request::loadPageLevels(int page) {
    std::vector<std::string> names = Cache::getLevelNames();

    Cache::setCount(0);

    for (int i = page * levelsPerPage; i <= page * levelsPerPage + levelsPerPage - 1; i++)
        if (i >= names.size()) Cache::addCount();
    
    for (int i = page * levelsPerPage; i <= page * levelsPerPage + levelsPerPage - 1; i++) {
        if (i >= names.size()) continue;

        if (Cache::getLevelId(i) != 0) {
            Cache::addCount();

            if (Cache::getCount() >= levelsPerPage)
                if (GDCPListLayer* layer = Utils::getLayer())
                    layer->loadPage(getLevelsString(page));

            continue;
        }

        auto req = web::WebRequest();
        req.header("Content-Type", "application/json");

        std::string name = names[i];

        Utils::replace(name, ' ', "%20");

        auto listener = std::make_shared<async::TaskHolder<web::WebResponse>>();

        listener->spawn(
            req.get(fmt::format("{}{}.json", listLink, name)),
            [listener, i, page](web::WebResponse value) {
                auto res = value.json();
                GDCPListLayer* layer = Utils::getLayer();

                if (res.isErr()) {
                    if (layer) layer->showError();

                    return log::error("1. Failed to load page levels: {}", res.unwrapErr());
                }

                auto json = res.unwrap();

                int id = json["id"].asInt().unwrapOr(0);

                if (id == 0) {
                    if (layer) layer->showError();

                    return log::error("2. Failed to load page levels: {}", res.unwrapErr());
                }

                Cache::addCount();
                Cache::setLevelId(i, id);

                if (Cache::getCount() >= levelsPerPage && layer)
                    layer->loadPage(getLevelsString(page));
        });
    }
}

std::string Request::getLevelsString(int page) {
    std::string str;
    log::info("getLevelsString called. Page: {}", page);

    // Log the size of the level names in Cache
    log::info("Cache::getLevelNames() size: {}", Cache::getLevelNames().size());

    for (int i = page * levelsPerPage; i <= page * levelsPerPage + levelsPerPage - 1; i++) {
        log::info("Processing index: {}", i);
        
        // Check if the index is within the valid range
        if (i >= Cache::getLevelNames().size()) {
            log::warn("Index {} is out of range. Skipping.", i);
            continue;
        }

        std::string name = Cache::getLevelNames()[i];
        log::info("Level name at index {}: {}", i, name);

        // Fetch the level ID
        int id = Cache::getLevelId(i);
        log::info("Level ID at index {}: {}", i, id);

        // Check if the ID is valid
        if (id == 0) {
            log::error("Failed to find level ID for index {}. ID is 0. Returning empty string.", i);
            return "";
        }

        // Add ID to the result string
        str += std::to_string(id) + (i == page * levelsPerPage + levelsPerPage - 1 ? "" : ",");
        log::info("Current result string: {}", str);
    }

    log::info("Returning final string: {}", str);
    return str;
}

/// @brief Platformer Levels

void Request::loadPagePlat(int page) {
    if (Cache::getLevelNamesPlat().empty())
        return loadLevelNamesPlat(true, page);

    loadPageLevelsPlat(page);
}

void Request::loadLevelNamesPlat(bool shouldLoadLevels, int page) {

    if (!Cache::getLevelNamesPlat().empty() && shouldLoadLevels)
        return loadPageLevelsPlat(page);

    auto req = web::WebRequest();

    req.header("Content-Type", "application/json");

    auto listener = std::make_shared<async::TaskHolder<web::WebResponse>>();

    listener->spawn(
        req.get(fmt::format("{}{}.json", platListLink, "_list")),
        [listener, shouldLoadLevels, page](web::WebResponse value) {
            auto res = value.json();
            GDCPListLayer* layer = Utils::getLayer();

            if (res.isErr()) {
                if (layer) layer->showError();
                
                return log::error("1. Failed to load level names: {}", res.unwrapErr());
            }

            auto json = res.unwrap().asArray();

            if (json.isErr()) {
                if (layer) layer->showError();
                
                return log::error("2. Failed to load level names: {}", json.unwrapErr());
            }

            std::vector<std::string> names;
            std::vector<std::string> realnames;

            for (auto& value : json.unwrap()) {
                std::string name = value.asString().unwrapOr("");
                if (name.empty()) continue;
                names.push_back(name);
            }

            Cache::setLevelNamesPlat(names);

            if (shouldLoadLevels)
                loadPageLevelsPlat(page);
    });
}

void Request::loadPageLevelsPlat(int page) {
    std::vector<std::string> names = Cache::getLevelNamesPlat();

    Cache::setCount(0);

    for (int i = page * levelsPerPage; i <= page * levelsPerPage + levelsPerPage - 1; i++)
        if (i >= names.size()) Cache::addCount();
    
    for (int i = page * levelsPerPage; i <= page * levelsPerPage + levelsPerPage - 1; i++) {
        if (i >= names.size()) continue;

        if (Cache::getLevelIdPlat(i) != 0) {
            Cache::addCount();

            if (Cache::getCount() >= levelsPerPage)
                if (GDCPListLayer* layer = Utils::getLayer())
                    layer->loadPage(getLevelsStringPlat(page));

            continue;
        }

        auto req = web::WebRequest();
        req.header("Content-Type", "application/json");

        std::string name = names[i];

        Utils::replace(name, ' ', "%20");

        auto listener = std::make_shared<async::TaskHolder<web::WebResponse>>();

        listener->spawn(
            req.get(fmt::format("{}{}.json", platListLink, name)),
            [listener, i, page](web::WebResponse value) {
                auto res = value.json();
                GDCPListLayer* layer = Utils::getLayer();

                if (res.isErr()) {
                    if (layer) layer->showError();

                    return log::error("1. Failed to load page levels: {}", res.unwrapErr());
                }

                auto json = res.unwrap();

                int id = json["id"].asInt().unwrapOr(0);

                if (id == 0) {
                    if (layer) layer->showError();

                    if (res.isErr()) {
                        return log::error("2. Failed to load page levels: {}", res.unwrapErr());
                    }
                }

                Cache::addCount();
                Cache::setLevelIdPlat(i, id);

                if (Cache::getCount() >= levelsPerPage && layer)
                    layer->loadPage(getLevelsStringPlat(page));
        });
    }
}

std::string Request::getLevelsStringPlat(int page) {
    std::string str;
    log::info("getLevelsStringPlat called. Page: {}", page);

    // Log the size of the level names in Cache
    log::info("Cache::getLevelNamesPlat() size: {}", Cache::getLevelNamesPlat().size());

    for (int i = page * levelsPerPage; i <= page * levelsPerPage + levelsPerPage - 1; i++) {
        log::info("Processing index: {}", i);
        
        // Check if the index is within the valid range
        if (i >= Cache::getLevelNamesPlat().size()) {
            log::warn("Index {} is out of range. Skipping.", i);
            continue;
        }

        std::string name = Cache::getLevelNamesPlat()[i];
        log::info("Level name at index {}: {}", i, name);

        // Fetch the level ID
        int id = Cache::getLevelIdPlat(i);
        log::info("Level ID at index {}: {}", i, id);

        // Check if the ID is valid
        if (id == 0) {
            log::error("Failed to find level ID for index {}. ID is 0. Returning empty string.", i);
            return "";
        }

        // Add ID to the result string
        str += std::to_string(id) + (i == page * levelsPerPage + levelsPerPage - 1 ? "" : ",");
        log::info("Current result string: {}", str);
    }

    log::info("Returning final string: {}", str);
    return str;
}

/// @brief Loads editors

void Request::loadEditors(bool shouldUpdateButtons) {
    if (!Cache::getEditors().empty()) return;
    
    auto req = web::WebRequest();
    req.header("Content-Type", "application/json");

    auto listener = std::make_shared<async::TaskHolder<web::WebResponse>>();

    listener->spawn(
       req.get(fmt::format("{}{}.json", listLink, "_editors")),
        [listener, shouldUpdateButtons](web::WebResponse value) {
            auto res = value.json();

            if (res.isErr())
                return log::error("1. Failed to load editors: {}", res.unwrapErr());

            auto arr = res.unwrap().asArray();

            if (arr.isErr())
                return log::error("2. Failed to load editors: {}", arr.unwrapErr());

            std::unordered_map<Role, std::vector<EditorEntry>> names;
            std::vector<EditorEntry> editorEntries;

            for (const matjson::Value& value : arr.unwrap()) {
                std::string roleStr = value["role"].asString().unwrapOr("");
                std::string name = value["name"].asString().unwrapOr("");
                int accountID = value["accountID"].asInt().unwrap();

                if (roleStr.empty() || name.empty()) continue;

                Role role = Role::Other;
                if (roleMap.contains(roleStr))
                    role = roleMap.at(roleStr);

                EditorEntry editor = {name, accountID, role};

                names[role].push_back(editor);
                editorEntries.push_back(editor);
            }

            std::string editors;

            for (const auto& [role, roleNames] : names) {
                editors += fmt::format("<c{}>", roleColors.at(role));
                for (EditorEntry editor : roleNames) {
                    editors += fmt::format("{} ({}). [See profile](user:{})", editor.name, roleStrings.at(role), editor.accountID);
                    if (editor.name != roleNames.back().name) editors += "\n\n";
                }
                editors += "</c>\n\n";
            }

            Cache::setEditors(editors);
            Cache::setEditorsList(editorEntries);

            if (shouldUpdateButtons)
                if (GDCPListLayer* layer = Utils::getLayer())
                    layer->updateButtons();
    });
}

void Request::loadWeekly(bool shouldUpdate) {
    auto req = web::WebRequest();
    req.header("Content-Type", "application/json");

    auto listener = std::make_shared<async::TaskHolder<web::WebResponse>>();

    listener->spawn(
        req.get(fmt::format("{}{}", listLink, "_WEEKLY")),
        [listener, shouldUpdate](web::WebResponse value) {
            auto res = value.string();

            if (res.isErr())
                return log::error("1. Failed to load weekly: {}", res.unwrapErr());

            int id = geode::utils::numFromString<int>(res.unwrapOr("0")).unwrapOr(0);

            if (id == 0)
                return log::error("2. Failed to load weekly.");

            Cache::setCurrentWeekly(id);

            if (Cache::getLocalWeekly() == 0) Cache::setLocalWeekly(id);

            if (shouldUpdate && id != 0) {
                CCScene* scene = CCDirector::sharedDirector()->getRunningScene();
                if (WeeklyPopup* popup = scene->getChildByType<WeeklyPopup>(0))
                    popup->loadLevel();
            }
    });
}