#include <iostream>
#include <string>
#include <vector>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#define HTTP_GET "GET"
#define HTTP_POST "POST"
#define HTTP_PATCH "PATCH"
#define HTTP_DELETE "DELETE"

void handleIndex(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res) {
        res.setStatus(Poco::Net::HTTPResponse::HTTP_OK);    
        res.setContentType("text/html");
        std::ostream& wr = res.send();
        wr << "<h3>WolvenSpirit</h3>" << std::endl;
        wr.flush();
        return;
    }
    void handleGetData(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res) {

    }

void router (Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res, const std::string uri, const std::string method) {
        if (uri == "/" && method == HTTP_GET) { handleIndex(req,res);}
        else if (uri == "/data" && method == HTTP_GET) { handleGetData(req,res);}
        else {
        res.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);    
        res.setContentType("text/html");
        std::ostream& wr = res.send();
        wr << "<h3>We can't seem to find what you are looking for.</h3>" << std::endl;
        wr.flush();
        return;
        }
}