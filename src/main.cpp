#include <iostream>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/ThreadPool.h>
#include <string>
#include <vector>
#include <map>
#include "router.cpp"
#include "config.cpp"
//#include "db.cpp"

int SERVER_PORT;
Poco::JSON::Object::Ptr CONFIG;
Router HttpRouter;

class RequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
    virtual void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res)
    {
        HttpRouter.HandleRequest(req,res);
    };
    static int count;
};

class HandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
    virtual Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &)
    {
        return new RequestHandler;
    };
};

class Server : public Poco::Util::ServerApplication
{
protected:
    int main(const std::vector<std::string> &args)
    {
        auto capacity = CONFIG->get("threadPoolAddCapacity").convert<int>();
        Poco::ThreadPool pool;
        pool.addCapacity(capacity);
        Poco::Net::HTTPServer s(new HandlerFactory, pool, Poco::Net::ServerSocket(SERVER_PORT), new Poco::Net::HTTPServerParams);

        std::cout << "Starting server on :" << SERVER_PORT << std::endl
        << "Current threads: " << s.currentThreads() << std::endl
        << "Thread pool capacity: " << s.maxThreads() << std::endl
        << "Maximum concurrent connections: " << s.maxConcurrentConnections() << std::endl;

        s.start();
        Poco::Util::ServerApplication::waitForTerminationRequest();
        s.stop();
    };
};

int main(int n, char **args)
{
    DBQueries = loadQueries();
    CONFIG = getConfig();
    HttpRouter.Init();
    pg = PG();
    pg.Connect(CONFIG);
    pg.MigrateUp();
    std::string serverPort = CONFIG->get("serverPort").toString();

    SERVER_PORT = std::stoi(serverPort);

    Server s;
    return s.run(n, args);
}
