#include <catch2/catch_all.hpp>
#include "../src/config.cpp"
#include "../src/queries.cpp"
#include <stdio.h>

TEST_CASE("basic check #1") {
   auto cfg = getConfig();
SECTION("#1.1 config is valid") {

    REQUIRE(cfg->get("serverPort").convert<int>() > 3000);
    REQUIRE(cfg->get("dbHost").convert<std::string>() == "db");
    REQUIRE(cfg->get("dbPort").convert<int>() == 5432);
    REQUIRE(cfg->get("dbName").convert<std::string>() == "db01");
}

SECTION("#1.2 queries file is loaded") {
    Queries *q = loadQueries();

    REQUIRE(q->Select->get("itemAll").convert<std::string>() == "select name, price, data, created from item;");
}

}