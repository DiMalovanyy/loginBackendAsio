//#pragma once
//#include "fields_alloc.h"
//#include <iostream>
//#include <vector>
//#include <memory>
//#include <thread>
//#include <utility>
//#include <chrono>
//
//#include <boost/asio.hpp>
//#include <boost/beast.hpp>
//
//
//using namespace boost;
//
//class Service {
//private:
//    Service() {
//        
//    }
//    
//    
//    void Handle() {
//        std::cout << "Some work start" << std::endl;
//        std::this_thread::sleep_for(std::chrono::seconds(10));
//        std::cout << "Some work ended" << std::endl;
//    }
//    
//public:
//    
//    
//    
//};
//
//
//class Acceptor {
//public:
//    
//    Acceptor(asio::io_context& ioc, const size_t port)
//    : _ioc(ioc), _acceptor(ioc, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port)) {
//        
//    }
//    
//    void Run() {
//        
//    }
//    
//    
//    void Stop() {
//        
//    }
//    
//    
//private:
//    asio::io_context& _ioc;
//    asio::ip::tcp::acceptor _acceptor;
//    
//};
//
//
//
//class Server {
//    
//public:
//    Server() {
//        _work.reset(new asio::io_context::work(_ioc));
//    }
//    
//    
//    void Run(const size_t port, const size_t thread_pool_size) {
//        assert(thread_pool_size > 0);
//        
//        _acceptor.reset(new Acceptor(_ioc, port));
//        _acceptor -> Run();
//        
//        
//        for(size_t thread_index = 0; thread_index < thread_pool_size; ++thread_index) {
//            std::unique_ptr<std::thread> thread(new std::thread([this, &_ioc](){
//                _ioc.run();
//            }));
//            _thread_pool.push_back(std::move(thread));
//        }
//    }
//    
//    
//    void Stop() {
//        _acceptor -> Stop();
//        _ioc.stop();
//        
//        
//        
//        for (const auto& thread : _thread_pool) {
//            thread -> join();
//        }
//        
//    }
//    
//private:
//    
//    asio::io_context _ioc;
//    std::unique_ptr<asio::io_context> _work;
//    std::vector<std::unique_ptr<std::thread>> _thread_pool;
//    std::unique_ptr<Acceptor> _acceptor;
//    
//};
