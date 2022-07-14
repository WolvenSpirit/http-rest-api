#include <iostream>
#include <string>
#include <vector>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/JSON/Stringifier.h>
#include <functional>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <fstream>
#include <libpq-fe.h>
#include "db.cpp"
#include <math.h>

#define HTTP_GET "GET"
#define HTTP_POST "POST"
#define HTTP_PATCH "PATCH"
#define HTTP_DELETE "DELETE"

typedef std::map<std::string, std::function<void(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res)>> MUX;

void handleIndex(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res)
{
    res.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    res.setContentType("text/html");
    std::ostream &wr = res.send();
    wr << "<h3>WolvenSpirit</h3></br><p>via Router class</p>" << std::endl;
    wr.flush();
    return;
}
void getItems(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res)
{
    Poco::JSON::Stringifier serializer;
    Poco::JSON::Object data;
    auto result = pg.Select("select name, price, data, created from item;");
    for (auto r : result) {
        data.set("name",r["name"]);
        data.set("price",r["price"]);
        data.set("data",r["data"]);
        data.set("created",r["created"]);
        std::ostream &wr = res.send();
        data.stringify(wr);
        wr.flush();
        return;
    }
}
void insertItem(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res)
{   
    Poco::JSON::Parser parser;
    std::ostringstream streamBuf;
    std::istream &is = req.stream();
    streamBuf << is.rdbuf();
    const auto obj = parser.parse(streamBuf.str()).extract<Poco::JSON::Object::Ptr>();

    auto name = obj->get("name").convert<std::string>();
    auto price = obj->get("price").extract<std::float_t>();
    auto data = obj->get("data").convert<std::string>();
    std::cout << name << price << data << std::endl;
    try {
        pqxx::work w{*pg.connection};
        w.exec_params("insert into item (name,price,data) values ($1,$2,$3);",name,price,data);
        w.commit();
    } catch(const std::exception &err) {
        std::cerr << err.what() << std::endl;
    }
    req.clear();
    res.setStatus(Poco::Net::HTTPServerResponse::HTTP_OK);
    auto &wr = res.send();
    wr.flush();
    return;
}

class Router {
    protected:
    MUX mux;
    public:
    std::unique_ptr<std::map<std::string, std::function<void(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res)>>> muxPtr;
    void Init() {
        mux.insert(std::make_pair(std::string("/"),handleIndex));
        mux["/item"] = insertItem;
        muxPtr = std::make_unique<MUX>(mux);
    }
    void HandleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res) {
        const std::string uri = req.getURI();
        const std::string method = req.getMethod();
        std::cout
            << method << " "
            << uri << " "
            << req.clientAddress()
            << std::endl;

        for (MUX::iterator n = mux.begin();n != mux.end();n++) {
            if (n->first == uri) {
                n->second(req,res);
                return;
            }
        }
        res.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
        res.setContentType("text/html");
        std::ostream &wr = res.send();
        wr << "<h3>We can't seem to find what you are looking for.</h3>" << std::endl;
        wr.flush();
        return;
    }
};