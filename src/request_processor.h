#pragma once

#include <logger.h>
#include <utility>
#include <thread>
#include <chrono>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>


#include <boost/beast/http.hpp>


using namespace boost;

class RequestProcessor {
public:
    
    
    RequestProcessor(std::string && requestJson): _requestJson(requestJson) {}
    
    
    std::string getJsonResponse() const {
        return "";
    }
    
    
    
    template<typename Body>
    beast::http::response<Body> getResponse() const {
        beast::http::response<beast::http::empty_body> res;
        res.version(11);
        res.result(beast::http::status::ok);
        res.set(beast::http::field::server, "Http test");
        res.prepare_payload();
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return res;
    }
    
    
private:
    std::string _requestJson;
    rapidjson::Document _jsonResponse;
};
