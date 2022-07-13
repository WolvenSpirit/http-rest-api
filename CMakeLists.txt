cmake_minimum_required(VERSION 3.10.0)
project(MyProject VERSION 0.1.0)
include(CMakePrintHelpers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

include_directories(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-linux/include")

find_package(Poco CONFIG COMPONENTS Foundation Util Net Zip JSON)
# find_package(PostgreSQL REQUIRED)
find_package(OpenSSL CONFIG COMPONENTS SSL Crypto)
find_package(libpqxx CONFIG COMPONENTS pqxx) 

add_executable(main src/main.cpp)

target_link_libraries(main PUBLIC libpqxx::pqxx)
target_link_libraries(main PRIVATE PostgreSQL::PostgreSQL)
target_link_libraries(main PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(main PUBLIC Poco::Foundation Poco::Zip Poco::Util Poco::Net Poco::JSON) 
set(CMAKE_CXX_FLAG "${CMAKE_CXX_FLAG} -lssl -lcrypto")
#target_link_libraries(main
#                        PRIVATE libpqxx::pqxx
#                        PRIVATE PostgreSQL::PostgreSQL
#                        PRIVATE OpenSSL::SSL 
#                        PRIVATE OpenSSL::Crypto)
message("Linker flags: ${CMAKE_CXX_FLAG} CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}") 