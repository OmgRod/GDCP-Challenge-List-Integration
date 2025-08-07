#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <omgrod.tsl-api/include/TSL.hpp>

using namespace geode::prelude;

struct GDCPStaffList {
    EventListener<web::WebTask> m_listener;

    void getStaffList(std::string endpoint, std::function<void(matjson::Value)> callback) {
        web::WebRequest req;
        auto task = req.post(fmt::format("{}/_editors.json", endpoint));
        m_listener.bind([callback](web::WebTask::Event* e) {
            if (auto* value = e->getValue()) {
                auto jsonRes = value->json();
                if (jsonRes) {
                    auto jsonVal = jsonRes.unwrap();
                    if (jsonVal.isArray()) {
                        callback(jsonVal);
                    } else {
                        callback(matjson::Value::array());
                    }
                } else {
                    callback(matjson::Value::array());
                }
            } else {
                callback(matjson::Value::array());
            }
        });
        m_listener.setFilter(task);
    }
};

static GDCPStaffList s_list;

$on_mod(Loaded) {
    auto listSettings = std::make_shared<tsl::ListSettings>();
    listSettings->name = "GDCP Challenge List";
    listSettings->listID = "gdcp-classic";
    listSettings->endpoint = "https://raw.githubusercontent.com/DeceptiveGD/Geometry-Dash-Challenge-Progression/refs/heads/main/data";
    listSettings->mod = Mod::get();
    listSettings->levelsPerPage = 10;
    listSettings->weekly = true;
    listSettings->staffInfo = true;

    auto staffTeam = tsl::StaffTeam::create();

    s_list.getStaffList(listSettings->endpoint, [listSettings, staffTeam](matjson::Value json) {
        if (json.isArray()) {
            for (const auto& obj : json.asArray().unwrap()) {
                log::info("Staff: {}", obj.dump());
            }
        }
        listSettings->staff = staffTeam;
        tsl::List::create(listSettings.get());
    });
}
