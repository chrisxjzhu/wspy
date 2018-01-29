#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "net/proxy.cpp"

TEST_CASE("A complicated one", "[proxy]")
{
    net::proxy proxy_(net::proxy::http, "u-s.er:pi.-ass@web-proxy.abc.com:8080");
    REQUIRE(proxy_.username() == "u-s.er");
    REQUIRE(proxy_.password() == "pi.-ass");
    REQUIRE(proxy_.host() == "web-proxy.abc.com");
    REQUIRE(proxy_.port() == 8080);
}

TEST_CASE("no port", "[proxy]")
{
    bool except = false;

    try {
        net::proxy proxy_(net::proxy::http, "web-proxy.abc.com");
    } catch (const net::proxy_no_port& e) {
        except = true;
    }

    REQUIRE(except == true);
}
