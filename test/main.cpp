#include <catch2/catch_all.hpp>
#include "../src/config.cpp"
#include "../src/queries.cpp"
#include <stdio.h>
#include "http_client.cpp"

using namespace HTTPTestClient;

TEST_CASE("basic check #1") {
   auto cfg = getConfig();
SECTION("#1.1 config is valid") {

    REQUIRE(cfg->get("serverPort").convert<int>() > 3000);
    REQUIRE(cfg->get("dbHost").convert<std::string>() == "db");
    REQUIRE(cfg->get("dbPort").convert<int>() == 5432);
    REQUIRE(cfg->get("dbName").convert<std::string>() == "db01");
}

SECTION("#1.2 queries file is loaded") {
    auto q = loadQueries();

    REQUIRE(q->Select->get("itemAll").convert<std::string>() == "select name, price, data, created from item;");
}

}

TEST_CASE("#2 items resource") {
    auto cfg = getConfig();
    auto port = cfg->get("dbHost").convert<Poco::UInt16>();
    auto host = "http://localhost";
    
SECTION("#2.1 GET all items") {
    auto sess = Session(host,port,"GET","/items");
    sess.session->sendRequest(*sess.request);
    sess.session->receiveResponse(*sess.response);
    REQUIRE(sess.response->getStatus() == Poco::Net::HTTPServerResponse::HTTP_OK);
}

SECTION("#2.2 /") {
    auto sess = Session(host,port,"GET","/");
    sess.session->sendRequest(*sess.request);
    sess.session->receiveResponse(*sess.response);
    REQUIRE(sess.response->getStatus() == Poco::Net::HTTPServerResponse::HTTP_OK);
}

}