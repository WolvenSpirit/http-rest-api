cmake_minimum_required(VERSION 3.10.0)
project(MyProject VERSION 0.1.0)


find_package(Poco REQUIRED COMPONENTS Foundation Util Net Zip JSON)
find_package(libpqxx)

add_executable(main src/main.cpp)


target_link_libraries(main PUBLIC Poco::Foundation Poco::Zip Poco::Util Poco::Net Poco::JSON libpqxx::pqxx)

