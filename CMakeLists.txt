cmake_minimum_required(VERSION 3.10.0)
project(MyProject VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

include_directories(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-linux/include")

find_package(Poco REQUIRED COMPONENTS Foundation Util Net Zip JSON)
find_package(PostgreSQL REQUIRED)

add_executable(main src/main.cpp)


target_link_libraries(main PRIVATE PostgreSQL::PostgreSQL)
target_link_libraries(main PUBLIC Poco::Foundation Poco::Zip Poco::Util Poco::Net Poco::JSON) 
target_link_libraries(main PRIVATE pq)