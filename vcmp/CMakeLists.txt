cmake_minimum_required(VERSION 3.10)
project(vcmp_client)

find_package(OpenSSL REQUIRED)
find_package(Libevent REQUIRED)

add_executable(vcmp_client main.c src/api.c src/listener.c lib/log.c lib/yyjson.c src/protocol.c src/rsa.c src/util/randdata.c)
target_include_directories(vcmp_client PUBLIC include)
target_link_libraries(vcmp_client PRIVATE libevent::core uuid wsockets OpenSSL::Crypto)