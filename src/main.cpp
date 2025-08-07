#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <omgrod.tsl-api/include/TSL.hpp>

using namespace geode::prelude;

class GDCPStaffList : public CCObject {
public:
    EventListener<web::WebTask> m_listener;

    void getStaffList(std::string endpoint, std::function<void(matjson::Value)> callback) {
        web::WebRequest req;
        auto task = req.post(fmt::format("{}/_editors.json", endpoint));
        m_listener.bind([callback](web::WebTask::Event* e) {
            if (auto* value = e->getValue()) {
                auto jsonRes = value->json();
                if (jsonRes && jsonRes.unwrap().isArray()) {
                    callback(jsonRes.unwrap());
                } else {
                    callback(matjson::Value::array());
                }
            } else {
                callback(matjson::Value::array());
            }
        });
        m_listener.setFilter(task);
    }

	static GDCPStaffList* create() {
		auto list = new GDCPStaffList();
		return list;
	}
};

static Ref<GDCPStaffList> s_list = GDCPStaffList::create();

$execute {
    auto listSettings = std::make_shared<tsl::ListSettings>();
    listSettings->name = "GDCP Challenge List";
    listSettings->listID = "gdcp-classic";
    listSettings->endpoint = "https://raw.githubusercontent.com/DeceptiveGD/Geometry-Dash-Challenge-Progression/refs/heads/main/data";
    listSettings->mod = Mod::get();
    listSettings->levelsPerPage = 10;
    listSettings->weekly = true;
    listSettings->staffInfo = true;

    auto staffTeam = tsl::StaffTeam::create();

    s_list->getStaffList(listSettings->endpoint, [listSettings, staffTeam](matjson::Value json) {
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

        listSettings->staff = staffTeam;
        tsl::List::create(listSettings.get());
    });
}
