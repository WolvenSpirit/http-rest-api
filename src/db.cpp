#include <iostream>
#include <string>
#include <libpq-fe.h>
#include <Poco/JSON/Object.h>
#include <Poco/Foundation.h>
#include <pqxx/pqxx> 
#include <fstream>
#include <iostream>

// Class DB is meant to be used statically and it is expected to have a lifespan equal to that of the program.
    class PG {
        private:
        public:
        pqxx::connection* connection;
        Poco::SharedPtr<pqxx::connection, Poco::ReferenceCounter, Poco::ReleasePolicy<pqxx::connection> > db;
        PG() {}
        void Connect(const Poco::JSON::Object::Ptr &Config) {
            auto name = Config->get("dbName").toString();
            auto host = Config->get("dbHost").toString();
            auto port = Config->get("dbPort").convert<std::string>();
            auto user = Config->get("dbUser").toString();
            auto pass = Config->get("dbPass").toString();
            std::string uri = "postgresql://"+user+":"+pass+"@"+host+":"+port+"/"+name;
            connection = new pqxx::connection(uri.c_str());
            db.assign(connection);

            if ((*connection).is_open()) {
                std::cout << "Connected using " << (*connection).connection_string() << std::endl;
                return;
            } else {
                 std::cout << "Connection to " << (*connection).dbname() << " not established" << std::endl;
                 exit(1);
            }

        } 

        pqxx::result Select(std::string query) {
            auto conn = db.get();
            pqxx::work w{*conn};
            pqxx::result result = w.exec(query);
            w.commit();
            return result;
        }

        void Exec(const std::string query, std::vector<std::string> args) {
            try {
                auto conn = db.get();
                if (conn == NULL || !(*conn).is_open()) {
                    std::cerr << "Database connection closed" << std::endl;
                }
                pqxx::work tx{*conn};
                auto result = tx.exec_params(query,args);
                tx.commit();
            } catch(const std::exception &err) {
                std::cerr << err.what() << std::endl;
            }
            // ...
        }

        void MigrateUp() {
            auto conn = db.get();
            pqxx::work w{*conn};
            std::ifstream fl("src/migrations/tables.sql",std::ios_base::in);
            std::string line;
            std::cout << "Running migrations..." << std::endl;
            try {
                while (std::getline(fl,line)) {
                    std::cout << "Adding table: " << line << std::endl;
                    w.exec(line);
                }
                w.commit();
            } catch(const std::exception &err) {
                std::cerr <<err.what() << std::endl;
            }
            fl.close();
        }
        ~PG() {
        }
    };

    PG pg;