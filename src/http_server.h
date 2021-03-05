#pragma once

#include <iostream>
#include <vector>
#include <deque>
#include <memory>
#include <thread>
#include <utility>
#include <chrono>
#include <optional>
#include <type_traits>
#include <algorithm>
#include <cstdlib>
#include <functional>


#include <spdlog/spdlog.h>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "request_processor.h"

using namespace boost;

namespace server {

// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template< class Body, class Allocator, class Send>
void handleRequest(beast::http::request<Body, beast::http::basic_fields<Allocator>>&& req, Send&& send) {
    spdlog::get("info_logger") -> info("The request start processing...");
    
    RequestProcessor processor("");
    beast::http::response<beast::http::empty_body> some = processor.getResponse<beast::http::empty_body>();
    
    send(std::move(some));
}



void fail(const beast::error_code& error_code, const std::string& message) {
    spdlog::get("error_logger") -> error("code = {0}. Message: {1}. With lable: {2}", error_code.value(), error_code.message(), message);
}

// Echoes back all received WebSocket messages
class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
public:
    
    //Take ownership of the socket
    explicit WebSocketSession( asio::ip::tcp::socket&& socket) : _socketStream(std::move(socket)) {}
    
    //Start the async accept operation
    template <typename Body, typename Allocator>
    void doAccept(beast::http::request<Body, beast::http::basic_fields<Allocator>> request) {
        // Set suggested timeout settings for the websocket
        _socketStream.set_option
            ( beast::websocket::stream_base::timeout::suggested( beast::role_type::server ) );
        
        // Set a decorator to change the Server of the handshake
        _socketStream.set_option(beast::websocket::stream_base::decorator(
            [](beast::websocket::response_type& res) {
                res.set(beast::http::field::server, "Hello from server");
            }));
        
        _socketStream.async_accept( request,
                                    beast::bind_front_handler( &WebSocketSession::onAccept, shared_from_this() ) );
    }
    
private:
    
    void onAccept(beast::error_code error_code) {
        if(error_code) {
            fail(error_code, "Accept"); return;
        }
        doRead();
    }
    
    void doRead() {
        //Read message into our buffer
        _socketStream.async_read(
                    _dynamicBuffer,
                    beast::bind_front_handler(
                        &WebSocketSession::onRead,
                        shared_from_this()));
    }
    
    void onRead(beast::error_code error_code, [[maybe_unused]] size_t bytes_transfered ) {
        // This indicates that the websocket_session was closed
        if(error_code == beast::websocket::error::closed) {
            return;
        } else if(error_code) {
            fail(error_code, "read");
        }
        
        // Echo the message
        _socketStream.text(_socketStream.got_text());
        _socketStream.async_write(
                    _dynamicBuffer.data(),
                    beast::bind_front_handler(
                        &WebSocketSession::onWrite,
                        shared_from_this()));
        
    }
    
    void onWrite(beast::error_code error_code, [[maybe_unused]] size_t bytes_transfered) {
        if(error_code) {
            fail(error_code, "write");
        }
        _dynamicBuffer.consume(_dynamicBuffer.size()); // Clear the buffer
        doRead(); // Do another read
    }
    
    beast::websocket::stream<beast::tcp_stream> _socketStream;
    beast::flat_buffer _dynamicBuffer;
};




class HttpSession: public std::enable_shared_from_this<HttpSession> {
public:
    //Take ownership of the socket
    HttpSession(asio::ip::tcp::socket&& socket)
        : _socketStream(std::move(socket)), _queue(*this) {}
    

    void Run() {
        doRead();
    }
    
private:
    
    void doRead() {
        _parser.emplace();  //Construct new parser for each message
        _parser->body_limit(10000);  // Apply a reasonable limit to the allowed size of the body in bytes to prevent abuse.
        _socketStream.expires_after(std::chrono::seconds(30));  //Set the timeouts
        
        // Read a request using the parser-oriented interface
        beast::http::async_read( _socketStream, _dynamicBuffer, *_parser,
                                beast::bind_front_handler(&HttpSession::onRead, shared_from_this()));
    }
    
    void onRead(beast::error_code error_code, [[maybe_unused]] size_t bytes_transfered) {
        if(error_code == beast::http::error::end_of_stream) {  // This means they closed the connection
            return doClose();
        } else if(error_code) { //Other fail
            return fail(error_code, "read");
        }
        
        // See if it is a WebSocket Upgrade
        if (beast::websocket::is_upgrade(_parser -> get())) {
            // Create a websocket session, transferring ownership
            // of both the socket and the HTTP request.
            std::make_shared<WebSocketSession>
            (_socketStream.release_socket()) -> doAccept(_parser -> release());
            return;
        }
        
        //Send the response
        handleRequest(std::move(_parser->release()), _queue);
        
        // If we aren't at the queue limit, try to pipeline another request
        if(! _queue.isFull()) {
            doRead();
        }
    }
    
    void OnWrite(bool close, beast::error_code error_code, [[maybe_unused]] size_t bytes_transfered) {
        if (error_code) {
            return fail(error_code, "write");
        }
        if (close) {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            doClose();
        }
        // Inform the queue that a write completed
        if(_queue.onWrite()) {
            doRead(); // Read another request
        }
    }
    
    void doClose() {
        //Send a TCP shutdown
        beast::error_code error_code;
        _socketStream.socket().shutdown(asio::ip::tcp::socket::shutdown_send, error_code);
        if (error_code) {
            fail(error_code, "close");
        }
        // At this point the connection is closed gracefully
    }
    
    class Queue {
    public:
        explicit Queue(HttpSession& selfSession): _selfSession(selfSession) {
            static_assert(LIMIT > 0, "limit must be positive");
        }
        
        //Returns true if we reached the end of the queue
        bool isFull() const {
            return _items.size() >= LIMIT;
        }
        
        //Called when a message finished read
        //Returns true if the caller should initiate a read
        bool onWrite() {
            BOOST_ASSERT(!_items.empty());
            const bool wasFull = isFull();
            _items.pop_front();
            if (!_items.empty()) {
                (*_items.front())();
            }
            return wasFull;
        }
        
        //Called by Http handler to send the response
        template<bool isRequest, typename Body, typename Fields>
        void operator()(beast::http::message<isRequest, Body, Fields>&& message) {
            //This holds a work item
            struct WorkImpl : Work {
                HttpSession& _selfSession;
                beast::http::message<isRequest, Body, Fields> _message;
                
                WorkImpl(HttpSession& selfSession, beast::http::message<isRequest, Body, Fields>&& message)
                    : _selfSession(selfSession), _message(std::move(message)) {}
                
                void operator() () {
                    beast::http::async_write(_selfSession._socketStream, _message,
                            beast::bind_front_handler(&HttpSession::OnWrite, _selfSession.shared_from_this(), _message.need_eof()));
                }
            };
            
            _items.push_back(std::make_unique<WorkImpl>(_selfSession, std::move(message)));
            
            // If there was no previous work, start this one
            if(_items.size() == 1)
                (*_items.front())();
        }
        
    private:
        static constexpr size_t LIMIT = 8; //Max namber of session that will queued
        struct Work { //Interface that stored
            virtual ~Work() = default;
            virtual void operator()() = 0;
        };
        HttpSession& _selfSession;
        std::deque<std::unique_ptr<Work>> _items;
    };
    
    beast::tcp_stream _socketStream;
    beast::flat_buffer _dynamicBuffer;
    
    Queue _queue;
    
    std::optional<beast::http::request_parser<beast::http::string_body>> _parser;
    
};


//Acceptor accept incoming connectiond and initiate a session
class Acceptor: public std::enable_shared_from_this<Acceptor> {
public:
    
    Acceptor(asio::io_context& ioc, const size_t port_num)
    : _ioc(ioc), _acceptor(asio::make_strand(ioc)) {
        beast::error_code error_code;
        asio::ip::tcp::endpoint endpoint(asio::ip::address_v4::any(), port_num);
        
        _acceptor.open(endpoint.protocol(), error_code);
        if(error_code) {
            fail(error_code, "open"); return;
        }
        _acceptor.set_option(asio::socket_base::reuse_address(true), error_code);
        if(error_code) {
            fail(error_code, "set address"); return;
        }
        _acceptor.bind(endpoint, error_code);
        if(error_code) {
            fail(error_code, "bind"); return;
        }
        //Start listening for connections
        _acceptor.listen(asio::socket_base::max_listen_connections, error_code);
        spdlog::get("info_logger") -> info("Acceptor start listening on port {} ", port_num);
        if (error_code) {
            fail(error_code, "listen");
        }
    }
    
    //Start incomingg connections
    void Run() {
        spdlog::get("info_logger") -> info("Acceptor start acceptions...");
        doAccept();
    }
    
private:
    
    void doAccept() {
        _acceptor.async_accept(
            asio::make_strand(_ioc),
            beast::bind_front_handler(
                &Acceptor::onAccept,
                shared_from_this()));
    }
    
    void onAccept(beast::error_code error_code, asio::ip::tcp::socket socket) {
        if(error_code) {
            fail(error_code, "accept");
        } else {
            // Create the http session and run it
            std::make_shared<HttpSession>(
                std::move(socket))->Run();
        }
        // Accept another connection
        doAccept();
    }
    
    
    
    asio::io_context &_ioc;
    asio::ip::tcp::acceptor _acceptor;
};


class Server {
public:
    
    Server(const short thread_pool_size) : _ioc(thread_pool_size), _thread_pool_size(thread_pool_size) {
        assert(thread_pool_size > 0);
        _thread_pool.reserve(thread_pool_size - 1);
    }
    
    void Run(const short port_num) {
        _acceptor.reset(new Acceptor(_ioc, port_num));
        _acceptor -> Run();
        
        
        // Capture SIGINT and SIGTERM to perform a clean shutdown
        asio::signal_set signals(_ioc, SIGINT, SIGTERM);
        signals.async_wait(
            [&](beast::error_code const&, int) {
                _ioc.stop();
            });
        
        for (short thread_index = _thread_pool_size - 1; thread_index > 0; --thread_index) {
            _thread_pool.emplace_back(new std::thread([_ioc = &_ioc](){
                _ioc -> run();
            }));
        }
        _ioc.run();
        
        // (If we get here, it means we got a SIGINT or SIGTERM)
        
        // Block until all the threads exit
        for (const auto& thread : _thread_pool) {
            thread -> join();
        }
        
    }

private:
    asio::io_context _ioc;
    const short _thread_pool_size;
    std::vector<std::unique_ptr<std::thread>> _thread_pool;
    std::shared_ptr<Acceptor> _acceptor;
};

}; //namespace server
