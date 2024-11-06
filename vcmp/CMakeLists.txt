cmake_minimum_required(VERSION 3.10)
project(vcmp_client)

find_package(OpenSSL REQUIRED)
find_package(Libevent REQUIRED)

find_package(PkgConfig REQUIRED)

pkg_search_module(UUID REQUIRED uuid)

add_executable(vcmp_client main.c
    src/api.c
    src/websockets/websockets.c
    src/websockets/frame.c
    src/websockets/handshake.c

    src/listener.c
    src/protocol.c
    src/rsa.c
    src/socks5.c

    src/util/randdata.c

    lib/log.c
    lib/ini.c
    lib/yyjson.c
)
target_include_directories(vcmp_client PUBLIC include ${UUID_INCLUDE_DIRS})
target_link_libraries(vcmp_client PRIVATE libevent::core ${UUID_LIBRARIES} OpenSSL::Crypto)

enable_testing()

add_executable(test_rsa tests/test_rsa.c)
target_link_libraries(test_rsa PRIVATE OpenSSL::Crypto)
add_test(NAME test_rsa COMMAND $<TARGET_FILE:test_rsa>)

# Requires TOR

# add_executable(test_socks5 tests/test_socks5.c src/socks5.c lib/log.c)
# target_include_directories(test_socks5 PUBLIC include)
# target_link_libraries(test_socks5 PRIVATE libevent::core)
# add_test(NAME test_socks5 COMMAND $<TARGET_FILE:test_socks5>)