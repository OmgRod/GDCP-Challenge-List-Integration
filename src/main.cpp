#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/utils/async.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <omgrod.tsl-api/include/TSL.hpp>

using namespace geode::prelude;
using namespace tsl;

class GDCPStaffList : public CCObject {
public:
    async::TaskHolder<web::WebResponse> m_listener;

    void getStaffList(std::string endpoint, std::function<void(matjson::Value)> callback) {
        web::WebRequest req;

        req.header("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:122.0) Gecko/20100101 Firefox/122.0");
        req.header("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
        req.header("Accept-Language", "en-US,en;q=0.5");
        req.header("Connection", "keep-alive");

        req.onProgress([](web::WebProgress const& progress) {
            log::debug("Progress: {}", progress.downloadProgress());
        });

        auto called = std::make_shared<std::atomic<bool>>(false);
        constexpr int timeoutMs = 5000;
        std::thread([called, callback, timeoutMs]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(timeoutMs));
            if (!called->exchange(true)) {
                log::error("Staff list request timed out after 5 seconds");
                callback(matjson::Value::array());
            }
        }).detach();

        m_listener.spawn(
            req.get(fmt::format("{}/_editors.json", endpoint)),
            [callback, called](web::WebResponse value) {
                if (called->exchange(true)) return;
                auto jsonRes = value.json();
                if (!value.ok()) {
                    log::error("Staff list web request failed: {}", value.string());
                    callback(matjson::Value::array());
                    return;
                }
                if (jsonRes && jsonRes.unwrap().isArray()) {
                    callback(jsonRes.unwrap());
                } else {
                    log::error("Staff list response was not a valid array");
                    callback(matjson::Value::array());
                }
            }
        );
    }

	static GDCPStaffList* create() {
		auto list = new GDCPStaffList();
		return list;
	}
};

static Ref<GDCPStaffList> s_list = GDCPStaffList::create();

$on_mod(Loaded) {
    /// CLASSIC LIST
    {
        auto listSettings = std::make_shared<ListSettings>();
        listSettings->name = "GDCP Challenge List";
        listSettings->listID = "classic"_spr;
        listSettings->endpoint = "https://raw.githubusercontent.com/DeceptiveGD/Geometry-Dash-Challenge-Progression/refs/heads/main/data";
        listSettings->mod = Mod::get();
        listSettings->levelsPerPage = 10;
        listSettings->weekly = true;
        listSettings->staffInfo = true;
        listSettings->icon = geode::createModLogo(Mod::get());
        listSettings->iconSmall = CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");

        auto regList = List::create(listSettings.get());
        if (regList) {
            log::info("List '{}' registered immediately.", listSettings->listID);
        } else {
            log::error("Failed to register list '{}'!", listSettings->listID);
        }

        auto staffTeam = StaffTeam::create();

        s_list->getStaffList(listSettings->endpoint, [regList, staffTeam](matjson::Value json) {
            if (json.isArray()) {
                for (const auto& obj : *json.asArray()) {
                    std::string role = obj.contains("role") ? obj["role"].asString().unwrapOr("unknown") : "unknown";
                    std::string name = obj.contains("name") ? obj["name"].asString().unwrapOr("unknown") : "unknown";
                    std::string link = obj.contains("link") ? obj["link"].asString().unwrapOr("none") : "none";
                    int accountID = obj.contains("accountID") ? obj["accountID"].asInt().unwrapOr(0) : 0;
                    log::info("Staff - Role: {}, Name: {}, Link: {}, ID: {}", role, name, link, accountID);
                }
            } else {
                log::info("JSON was not an array.");
            }

            if (regList) regList->m_settings->staff = staffTeam;
        });

        ListRegistry::registerList(regList);
    }

    /// PLATFORMER LIST
    {
        auto listSettings = std::make_shared<ListSettings>();
        listSettings->name = "GDCP Challenge List";
        listSettings->listID = "platformer"_spr;
        listSettings->endpoint = "https://raw.githubusercontent.com/DeceptiveGD/GDCP-Platformer-List/refs/heads/main/data";
        listSettings->mod = Mod::get();
        listSettings->levelsPerPage = 10;
        listSettings->weekly = true;
        listSettings->staffInfo = true;
        listSettings->icon = geode::createModLogo(Mod::get());
        listSettings->iconSmall = CCSprite::createWithSpriteFrameName("GJ_moonsIcon_001.png");

        auto* regList = List::create(listSettings.get());
        if (regList) {
            log::info("List '{}' registered immediately.", listSettings->listID);
        } else {
            log::error("Failed to register list '{}'!", listSettings->listID);
        }

        auto staffTeam = StaffTeam::create();

        s_list->getStaffList(listSettings->endpoint, [regList, staffTeam](matjson::Value json) {
            if (json.isArray()) {
                for (const auto& obj : *json.asArray()) {
                    std::string role = obj.contains("role") ? obj["role"].asString().unwrapOr("unknown") : "unknown";
                    std::string name = obj.contains("name") ? obj["name"].asString().unwrapOr("unknown") : "unknown";
                    std::string link = obj.contains("link") ? obj["link"].asString().unwrapOr("none") : "none";
                    int accountID = obj.contains("accountID") ? obj["accountID"].asInt().unwrapOr(0) : 0;
                    log::info("Staff - Role: {}, Name: {}, Link: {}, ID: {}", role, name, link, accountID);
                }
            } else {
                log::info("JSON was not an array.");
            }

            if (regList) regList->m_settings->staff = staffTeam;
        });

        ListRegistry::registerList(regList);
    }
}
