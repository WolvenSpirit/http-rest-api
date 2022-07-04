cmake_minimum_required(VERSION 3.10.0)
project(MyProject VERSION 0.1.0)


find_package(Poco REQUIRED COMPONENTS Foundation Util Net Zip )

add_executable(main main.cpp)


target_link_libraries(main PUBLIC Poco::Foundation Poco::Zip Poco::Util Poco::Net)


