#include <iostream>
#include <fstream>
#include <string>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>

class Config
{
public:
    class init
    {
    private:
        std::fstream fl;
        std::ostringstream buf;
        Poco::JSON::Parser parser;
    public:
        Poco::JSON::Object::Ptr config;

        init()
        {
            fl.open("config.json", std::ios_base::in);

            if (fl.is_open())
            {
                buf << fl.rdbuf();
            }
            Poco::Dynamic::Var dynVar = parser.parse(buf.str());
            config = dynVar.extract<Poco::JSON::Object::Ptr>();
        }

        ~init()
        {
            fl.close();
        }
    };
};

Poco::JSON::Object::Ptr getConfig() {
    Config::init c;
    return c.config;
}