#include <iostream>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/ServerApplication.h>
#include <string>
#include <vector>
#include <map>
#include "router.cpp"
#include "config.cpp"

int SERVER_PORT;

class RequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
    virtual void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res)
    {
        const std::string uri = req.getURI();
        const std::string method = req.getMethod();
        std::cout
            << method << " "
            << uri << " "
            << req.clientAddress()
            << std::endl;

        router(req, res, uri, method);
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
        Poco::Net::HTTPServer s(new HandlerFactory, Poco::Net::ServerSocket(SERVER_PORT), new Poco::Net::HTTPServerParams);
        s.start();
        Poco::Util::ServerApplication::waitForTerminationRequest();
        s.stop();
    };
};

int main(int n, char **args)
{
    Poco::JSON::Object::Ptr config = getConfig();
    std::string dbName = config->get("dbName").toString();
    std::string dbHost = config->get("dbHost").toString();
    std::string dbPort = config->get("dbPort").toString();
    std::string dbUser = config->get("dbUser").toString();
    std::string dbPass = config->get("dbPass").toString();
    std::string serverPort = config->get("serverPort").toString();

    if (n < 2)
    {
        std::cout << "server port required as argument" << std::endl;
        return 1;
    }
    else
    {
        SERVER_PORT = std::stoi(serverPort);
    }
    Server s;
    return s.run(n, args);
}
