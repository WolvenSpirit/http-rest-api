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
#include "queries.cpp"
#include "metric.cpp"

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

// Test endpoint
void getItems(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res)
{
    Poco::JSON::Array arr;
    res.setContentType("application/json");
    res.setStatus(Poco::Net::HTTPServerResponse::HTTP_OK);
    std::ostream &wr = res.send();
    auto query = DBQueries->Select->get("itemAll").convert<std::string>();
    std::cout << query << std::endl;
    pqxx::result result = pg.Select(query);
    for (auto r : result) {
        Poco::JSON::Object data(Poco::JSON_PRESERVE_KEY_ORDER);
        data.set("name",r["name"].c_str());
        data.set("price",r["price"].c_str());
        data.set("data",r["data"].c_str());
        data.set("created",r["created"].c_str());
        arr.add(data);
    }

    arr.stringify(wr);
    wr.flush();
    return;
}

// Test endpoint
void insertItem(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res)
{   
    Poco::JSON::Parser parser;
    std::ostringstream streamBuf;
    std::istream &is = req.stream();
    streamBuf << is.rdbuf();
    const auto obj = parser.parse(streamBuf.str()).extract<Poco::JSON::Object::Ptr>();

    auto name = obj->get("name").convert<std::string>();
    auto price = obj->get("price").convert<std::string>();
    auto data = obj->get("data").convert<std::string>();
    std::cout << name << price << data << std::endl;
    try {
        pqxx::work w{*pg.db.get()};
        std::string query = DBQueries->Insert->get("item").toString();
        w.exec_params(query,name,price,data);
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
        mux["/get/items"] = getItems;
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
                Metrics::requests_counter->Add({{"http_method",method},{"uri",uri}}).Increment();
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