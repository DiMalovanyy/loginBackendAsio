#include "http_server.h"
#include <thread>
#include "logger.h"





//#include "sync_http_service.h"


int main(int argc, char**argv) {
    Logger().defineCoutLogger("info_logger").defineErrorLogger("error_logger");
    
    server::Server httpServer(std::thread::hardware_concurrency());
    httpServer.Run(9939);
    
    return 0;
}
