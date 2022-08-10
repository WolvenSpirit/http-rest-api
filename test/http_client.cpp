#include <catch2/catch_all.hpp>
#include <stdio.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServerResponse.h>

class ClientSession
{
public:
    Poco::Net::HTTPClientSession *session;
    Poco::Net::HTTPRequest *request;
    Poco::Net::HTTPResponse *response;
    std::ostream *writable;
    ClientSession(
        Poco::Net::HTTPClientSession *sess,
        Poco::Net::HTTPRequest *req,
        Poco::Net::HTTPResponse *res,
        std::ostream *wr = NULL)
    {
        session = sess;
        request = req;
        response = res;
        wr = wr;
    }
};

namespace HTTPTestClient
{
    // Create a new client session and request and return the writable stream
    ClientSession Session(const std::string host, const Poco::UInt16 port, std::string method, std::string uri)
    {
        auto req = Poco::Net::HTTPRequest(method, uri);
        auto session = Poco::Net::HTTPClientSession(host, port);
        req.setContentType("application/json");
        auto res = Poco::Net::HTTPResponse();
        // std::ostream &wr =  session.sendRequest(req);
        // std::ostream *ostr = &wr;
        auto sess = ClientSession(&session, &req, &res);
        return sess;
    }
}