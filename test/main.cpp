#include <catch2/catch_all.hpp>
#include "../src/config.cpp"
#include "../src/queries.cpp"
#include <stdio.h>
#include "http_client.cpp"

TEST_CASE("basic check #1") {
   auto cfg = getConfig();
    SECTION("#1.1 config is valid") {

        REQUIRE(cfg->get("serverPort").convert<int>() > 3000);
        REQUIRE(cfg->get("dbHost").convert<std::string>() == "localhost");
        REQUIRE(cfg->get("dbPort").convert<int>() == 5432);
        REQUIRE(cfg->get("dbName").convert<std::string>() == "db01");
    }

    SECTION("#1.2 queries file is loaded") {
        auto q = loadQueries();

        REQUIRE(q->Select->get("itemAll").convert<std::string>() == "select name, price, data, created from item;");
    }

}

TEST_CASE("#2 items resource") {
    Poco::UInt16 port;
    auto cfg = getConfig();
    port = cfg->get("serverPort").convert<Poco::UInt16>();
    auto host = "localhost";
    auto session = Poco::Net::HTTPClientSession(host, port);

    SECTION("#2.1 GET all items") { 
        auto method = "GET";
        auto uri = "/get/items";
        auto request = Poco::Net::HTTPRequest(method, uri);
        request.setContentType("application/json");
        auto response = Poco::Net::HTTPResponse();

        session.sendRequest(request);
        session.receiveResponse(response);

        REQUIRE(response.getStatus() == Poco::Net::HTTPServerResponse::HTTP_OK);
        REQUIRE(response.getContentType() == "application/json");
        session.reset();
    }

}