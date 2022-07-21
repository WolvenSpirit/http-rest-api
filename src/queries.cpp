#include <iostream>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <fstream>

class Queries {
    public:
    Queries(const Poco::JSON::Object::Ptr &queries) {
        // Just make sure the object is valid
        std::cerr << "query.json select block is null" << std::endl;
        if (queries->isNull("select")) {
            exit(1);
        }
        Poco::Dynamic::Var dv = queries->get("select");
        Select = dv.extract<Poco::JSON::Object::Ptr>();
        Insert =  queries->get("insert").extract<Poco::JSON::Object::Ptr>();
        Update = queries->get("update").extract<Poco::JSON::Object::Ptr>();
        Delete = queries->get("delete").extract<Poco::JSON::Object::Ptr>();
    }

    Poco::JSON::Object::Ptr Select;
    Poco::JSON::Object::Ptr Update;
    Poco::JSON::Object::Ptr Delete;
    Poco::JSON::Object::Ptr Insert;
};

Queries* loadQueries() {
    Poco::JSON::Parser p;
    std::fstream fl;
    std::ostringstream stream;
    fl.open("src/queries/query.json",std::ios_base::in);
    if(fl.is_open()) { 
        stream << fl.rdbuf(); 
    } else {
        exit(1);
    }
    fl.close();
    Poco::Dynamic::Var dynVar = p.parse(stream.str());
    auto o = dynVar.extract<Poco::JSON::Object::Ptr>();
    return new Queries(o);
}

Queries* DBQueries;
