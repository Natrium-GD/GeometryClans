#ifndef WEB_REQUEST_TEMPLATE_H
#define WEB_REQUEST_TEMPLATE_H

#include <string>
#include <map>
#include <functional>
#include <matjson.hpp>
#include <Geode/utils/web.hpp> 
#include <Geode/loader/Event.hpp>
#include <Geode/Geode.hpp>

#include "globals.hpp"

using namespace geode::prelude;

namespace my_web {

    struct Fields {
        EventListener<web::WebTask> m_listener;
    };

    class WebRequestTemplate {
    public:
        WebRequestTemplate() : m_json(matjson::Value()) {}

        template<typename T>
        WebRequestTemplate& set(const std::string& key, const T& value) {
            m_json.set(key, value);
            return *this;
        }

        WebRequestTemplate& setEdit(bool edit) {
            m_json.set("edit", edit);
            return *this;
        }

        WebRequestTemplate& setRow(int row) {
            m_json.set("row", row);
            return *this;
        }

        void sendRequest(const std::string& url, const std::function<void()>& onSuccess) {
            geode::utils::web::WebRequest req = geode::utils::web::WebRequest()
                .bodyJSON(m_json)
                .header("Content-Type", "application/json");

            WebRequestTemplate::fields.m_listener.bind([onSuccess](geode::utils::web::WebTask::Event* e) {
                if (geode::utils::web::WebResponse* res = e->getValue()) {
                    onSuccess();
                }
                });

            WebRequestTemplate::fields.m_listener.setFilter(req.post(url));
        }

        void getSheetData(const std::string& url, const std::function<void()>& onSuccess) {
            geode::utils::web::WebRequest req = geode::utils::web::WebRequest()
                .header("Content-Type", "application/json");

            WebRequestTemplate::fields.m_listener.bind([onSuccess](geode::utils::web::WebTask::Event* e) {
                if (geode::utils::web::WebResponse* res = e->getValue()) {
                    auto responseResult = res->string();
                    std::string responseBody = responseResult.unwrap();
                    auto parseResult = matjson::parse(responseBody);
                    matjson::Value jsonData = parseResult.unwrap();

                    g_sheetData = jsonData["data"];
                    onSuccess();
                }
                });

            WebRequestTemplate::fields.m_listener.setFilter(req.get(url));
        }


    private:
        matjson::Value m_json;

        static Fields fields; 
    };

}

#endif 
