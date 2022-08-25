#include <iostream>
#include <string>
#include <libpq-fe.h>
#include <Poco/JSON/Object.h>
#include <Poco/Foundation.h>
#include <pqxx/pqxx>
#include <fstream>
#include <iostream>
#include <resource_pool.hpp>
#include <memory>

typedef pqxx::connection *pool_conn;
Pool<pool_conn> *pool;

// Class DB is meant to be used statically and it is expected to have a lifespan equal to that of the program.
class PG
{
private:
public:
    // Consumers of this class should use the pool instead of this root connection
    Poco::SharedPtr<pqxx::connection, Poco::ReferenceCounter, Poco::ReleasePolicy<pqxx::connection>> db;
    std::string uri;
    PG() {}
    void Connect(const Poco::JSON::Object::Ptr &Config)
    {
        auto name = Config->get("dbName").toString();
        auto host = Config->get("dbHost").toString();
        auto port = Config->get("dbPort").convert<std::string>();
        auto user = Config->get("dbUser").toString();
        auto pass = Config->get("dbPass").toString();
        uri = "postgresql://" + user + ":" + pass + "@" + host + ":" + port + "/" + name;
        db = new pqxx::connection(uri.c_str());

        if ((*db.get()).is_open())
        {
            std::cout << "Connected using " << (*db.get()).connection_string() << std::endl;
            return;
        }
        else
        {
            std::cout << "Connection to " << (*db.get()).dbname() << " not established" << std::endl;
            exit(1);
        }
    }

    pqxx::result Select(std::string query)
    {
        pqxx::result result;
        auto conn = pool->get();
        std::cout << conn->dbname() << " : " << query << std::endl;
        pqxx::work w{*conn};
        try
        {
            result = w.exec(query);
            w.commit();
            pool->add(conn);
        }
        catch (const std::exception &err)
        {
            w.commit();
            pool->add(conn);
            std::cerr << "Select: " << err.what() << std::endl;
        }
        return result;
    }

    template <typename... VariadicQueryArgument>
    void Exec(const std::string query, VariadicQueryArgument... args)
    {
        auto conn = pool->get();
        pqxx::work tx{*conn};
        try
        {
            auto result = tx.exec_params(query, args...);
            tx.commit();
            pool->add(conn);
        }
        catch (const std::exception &err)
        {
            tx.abort();
            pool->add(conn);
            std::cerr << err.what() << std::endl;
        }
        // ...
    }

    void MigrateUp()
    {
        auto conn = pool->get();
        pqxx::work w{*conn};
        std::ifstream fl("src/migrations/tables.sql", std::ios_base::in);
        std::string line;
        std::cout << "Running migrations..." << std::endl;
        try
        {
            while (std::getline(fl, line))
            {
                std::cout << "Adding table: " << line << std::endl;
                w.exec(line);
            }
            w.commit();
            pool->add(conn);
        }
        catch (const std::exception &err)
        {
            w.abort();
            pool->add(conn);
            std::cerr << "Failed migration: " << err.what() << std::endl;
        }
        fl.close();
    }
    ~PG()
    {
    }
};

PG pg;