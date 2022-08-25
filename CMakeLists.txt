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
find_package(prometheus-cpp CONFIG COMPONENTS core pull)
find_package(resource-pool REQUIRED)

# CMake Tools vscode extension errors without setting BUILD_CONTEXT to a default.
# However the file is valid without defining the HOST default if invoked via command-line.
# Will be extended with a TEST context
if(NOT DEFINED $ENV{BUILD_CONTEXT})
    set(ENV{BUILD_CONTEXT} HOST)
endif()

message("Running CmakeLists with BUILD_CONTEXT=$ENV{BUILD_CONTEXT}")

if($ENV{BUILD_CONTEXT} STREQUAL "DOCKER")
    message("Skipping test scope find_package lines")
else()
    find_package(Catch2 CONFIG COMPONENTS Catch2 Catch2WithMain)
endif()

add_executable(main src/main.cpp)

target_link_libraries(main PUBLIC libpqxx::pqxx)
target_link_libraries(main PRIVATE PostgreSQL::PostgreSQL)
target_link_libraries(main PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(main PUBLIC Poco::Foundation Poco::Zip Poco::Util Poco::Net Poco::JSON) 
target_link_libraries(main PUBLIC prometheus-cpp::core prometheus-cpp::pull)
target_link_libraries(main PUBLIC 
resource-pool::resource-pool
${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-linux/lib/libresource-pool.a
)
target_link_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/x64-linuxvcpkg/installed/x64-linux/lib/)

if($ENV{BUILD_CONTEXT} STREQUAL "DOCKER")
    message("Skipping test scope add_executable and linking")
else()
    add_executable(test test/main.cpp)
    add_test(NAME test COMMAND test)
    target_link_libraries(test PUBLIC Catch2::Catch2 Catch2::Catch2WithMain)
    target_link_libraries(test PUBLIC Poco::Foundation Poco::Zip Poco::Util Poco::Net Poco::JSON) 
    target_include_directories(test PUBLIC ${resource-pool_LIBRARIES})
    target_link_directories(test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/lib/)
endif()

set(CMAKE_CXX_FLAG "${CMAKE_CXX_FLAG} -lssl -lcrypto -lresource-pool")
message("Linker flags: ${CMAKE_CXX_FLAG} CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}") 