#include <iostream>
#include <string>
#include <libpq-fe.h>
#include <Poco/JSON/Object.h>
#include <Poco/Foundation.h>
#include <pqxx/pqxx> 

// Class DB is meant to be used statically and it is expected to have a lifespan equal to that of the program.
class DB {
    public:
    class PG {
        private:
        public:
        Poco::SharedPtr<pqxx::connection, Poco::ReferenceCounter, Poco::ReleasePolicy<pqxx::connection> > db;
        PG() {}
        void Connect(const Poco::JSON::Object::Ptr &Config) {
            auto name = Config->get("dbName").toString();
            auto host = Config->get("dbHost").toString();
            auto port = Config->get("dbPort").convert<std::string>();
            auto user = Config->get("dbUser").toString();
            auto pass = Config->get("dbPass").toString();
            std::string uri = "postgresql://"+user+":"+pass+"@"+host+":"+port+"/"+name;
            pqxx::connection connection(uri.c_str());
            db.assign(&connection);
            if (connection.is_open()) {
                std::cout << "Connected to " << connection.dbname() << std::endl;
                return;
            } else {
                 std::cout << "Connection to " << connection.dbname() << " not established" << std::endl;
                 exit(1);
            }

        } 

        void Exec(const std::string query, std::vector<char*> args) {
            auto conn = db.get(); 
            pqxx::work tx(*conn);
            tx.exec(query);
            // ...
        }
        ~PG() {
            
        }
    };
};

DB::PG pg;