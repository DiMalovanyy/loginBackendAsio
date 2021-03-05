#pragma once

#include <logger.h>
#include <utility>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <optional>
#include <string_view>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>


//#include <boost/beast/http/string_body.hpp>
//#include <boost/beast/http/message.hpp>
#include <boost/beast/http.hpp>

using namespace boost;

template <typename RequestBody>
class RequestProcessor {
public:
    
    RequestProcessor(beast::http::request<RequestBody>&& request): _request(std::move(request)) {
        parseRequest();
    }
    
//    template<typename Body>
//    beast::http::response<Body> getResponse() const {
//        beast::http::response<beast::http::empty_body> res;
//        res.version(11);
//        res.result(beast::http::status::ok);
//        res.set(beast::http::field::server, "C++ Beast Server");
//        res.prepare_payload();
//        std::this_thread::sleep_for(std::chrono::seconds(10));
//        return res;
//    }
    
    
    beast::http::response<beast::http::string_body> getResponse() const {
        beast::http::response<beast::http::string_body> response;
//        responseBuilder(response);
        
        
        response.version(11);
        response.result(beast::http::status::ok);
        response.set(beast::http::field::server, "Beast Http server");
        
        
        response.prepare_payload();
        return response;
    }
    
    
private:
    
    
    struct ResponseBuilder {
        std::string message;
        std::optional<std::pair<size_t, std::string>> error_code = std::nullopt;
        std::unordered_map<std::string, std::string> response_headers;
        
        void operator()(beast::http::response<beast::http::string_body>& response) {
            response.version(11);
            
            if (!error_code) {
                response.result(beast::http::status::ok);
            }
            
            typename beast::http::string_body::value_type& body = response.body();
            response.prepare_payload();
        }
        
    };
    
    void parseRequest() {
        beast::string_view target = _request.target();
        beast::string_view method = _request.method_string();
        
        if (_request.chunked()) { // check if the chunked Transfer-Encoding is specified
            
        }
        if (_request.keep_alive()) { //check if is it keep-alive connection
            
        }
        if (_request.has_content_length()) { //check if content_length field exist
            
        }
        if (_request.need_eof()) { //check if the message need end of file
            
        }
        
        typename RequestBody::value_type request_str = _request.body();
        spdlog::get("info_logger") -> info("Request: {0}. With target {1}. And method {2}",  request_str, std::string(target), std::string(method));
    }
    
    
    beast::http::request<RequestBody> _request;
    std::unordered_map<std::string, std::string> _requestHeaders;
    
    ResponseBuilder responseBuilder;
    
    std::string _responseMessage;
    std::string _requestJson;
    rapidjson::Document _jsonResponse;
};
