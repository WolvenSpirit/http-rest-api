#include <iostream>
#include <string>
#include <libpq-fe.h>
#include <Poco/JSON/Object.h>


class DB {
    public:
    class PG {
        private:
        PGconn *connection;
        public:
        Poco::SharedPtr<PGconn, Poco::ReferenceCounter, Poco::ReleasePolicy<PGconn> > db;
        PG() {}
        void Connect(Poco::JSON::Object::Ptr Config) {
            auto name = Config->get("dbName").toString();
            auto host = Config->get("dbHost").toString();
            auto port = Config->get("dbPort").convert<std::string>();
            auto user = Config->get("dbUser").toString();
            auto pass = Config->get("dbPass").toString();
            std::string uri = "postgresql://"+user+":"+pass+"@"+host+":"+port+"/"+name;
            connection = PQconnectdb((char*)uri.c_str());
            if (PQstatus(connection) != CONNECTION_OK) {
                std::cout << "Failed to connect to postgres" << std::endl 
                << uri << std::endl;
                exit(1);
            } else {
                std::cout << "Connected to postgres" << std::endl;
                // The raw pointer should be wrapped as a smart pointer when used outside the class
                db.assign(connection);
            }
        }
        ~PG() {
            
        }
    };
};

