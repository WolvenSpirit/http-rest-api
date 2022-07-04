#include <iostream>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/ServerApplication.h>
#include <iostream>
#include <string>
#include <vector>


class RequestHandler : public Poco::Net::HTTPRequestHandler {
    virtual void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res) {
        res.setStatus(Poco::Net::HTTPResponse::HTTP_OK);    
        res.setContentType("text/html");
        std::ostream& wr = res.send();
        wr << "<h3>WolvenSpirit</h3>" << std::endl;
        wr.flush();
        return;
    };
    static int count;
};


class HandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
    public:
    virtual Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest &) {
        return new RequestHandler;
    };
};


class Server : public Poco::Util::ServerApplication {
    protected:
    int main(const std::vector<std::string> &) {
            Poco::Net::HTTPServer s(new HandlerFactory, Poco::Net::ServerSocket(9005), new Poco::Net::HTTPServerParams);
            s.start();
            Poco::Util::ServerApplication::waitForTerminationRequest();
            s.stop();
    };
};


int main(int n, char** args) {
    int port = std::stoi(args[1]);
    Server s;
    return s.run(n,args);
}
