#include <iostream>
#include <string>
#include <vector>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/JSON/Stringifier.h>
#include <functional>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Net/MultipartReader.h>
#include <Poco/Net/NetException.h>
#include <fstream>
#include <libpq-fe.h>
#include "db.cpp"
#include <math.h>
#include "queries.cpp"
#include "metric.cpp"
#include <future>

#define HTTP_GET "GET"
#define HTTP_POST "POST"
#define HTTP_PATCH "PATCH"
#define HTTP_DELETE "DELETE"

typedef std::map<std::string, std::function<void(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res)>> MUX;

void incrementRC(const prometheus::Labels &labels) {
    return Metrics::requests_counter->Add(labels).Increment();
}

void handleIndex(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res, Poco::Net::HTTPServer *s = NULL)
{
    if (s == NULL) {
        res.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        res.setContentType("text/html"); 
        std::ostream &wr = res.send();
        wr << "<h3>Internal error</h3></br>" << std::endl;
        wr.flush();
        return;
    }
    res.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    res.setContentType("application/json");
    std::ostream &wr = res.send();
    Poco::JSON::Object data(Poco::JSON_PRESERVE_KEY_ORDER);
    data.set("name","WolvenSpirit's API");
    data.set("current_threads",s->currentThreads());
    data.set("current_connections",s->currentConnections());
    data.set("max_concurrent_connections", s->maxConcurrentConnections());
    data.stringify(wr);
    wr.flush();
    return;
}

void uploadFile(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res) {
    Poco::Net::MultipartReader reader(req.stream());
    // This needs to be hardened, doesn't check for mime, doesn't check size, doesn't limit maximum number of files
    while (reader.hasNextPart()) {
        try {
            Poco::Net::MessageHeader headerv;
            auto &header = headerv;
            reader.nextPart(header);
            const std::istream &ref = reader.stream();

            std::string end;
            Poco::Net::NameValueCollection params;
            // This Poco class has a rather weird usage pattern, not immediately obvious that it should be used like this
            header.splitParameters(header["Content-Disposition"],end,params);
            std::cout << params.get("filename","unnamed") << std::endl;

            // Instead of writing to disk, upload the buffer data to s3 or similar
            std::fstream fl;
            fl.open("uploads/"+params.get("filename","unnamed"),std::ios_base::out);
            fl << ref.rdbuf();
            fl.close();
        } catch (const std::exception &err) {
            std::cerr << err.what() << std::endl;
        }   
    }
    res.setStatus(Poco::Net::HTTPServerResponse::HTTP_OK);
    res.send().flush();
}

// Test endpoint
void getItems(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res)
{
    try {
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
    } catch (const std::exception &err) { std::cerr << "getItems:" << err.what() << std::endl; }
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
    Poco::Net::HTTPServer *s;
    std::unique_ptr<std::map<std::string, std::function<void(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res)>>> muxPtr;
    void Init() {
        mux["/item"] = insertItem;
        mux["/get/items"] = getItems;
        mux["/upload"] = uploadFile;
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
        incrementRC({
                        {"current_threads", std::to_string(s->currentThreads())},
                        {"current_connections", std::to_string(s->currentConnections())},
                        {"max_concurrent_connections", std::to_string(s->maxConcurrentConnections())}
                    });
        for (MUX::iterator n = mux.begin();n != mux.end();n++) {
            if (n->first == uri) {
                n->second(req,res);
                incrementRC({{"http_method",method},{"uri",uri}});
                return;
            }
        }
        if (uri == "/") {
            // Expose server stats
            handleIndex(req,res,s);
        }
        res.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
        res.setContentType("text/html");
        std::ostream &wr = res.send();
        wr << "<h3>We can't seem to find what you are looking for.</h3>" << std::endl;
        wr.flush();
        return;
    }
};