#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "net/url.cpp"

TEST_CASE("A complicated url", "[url]")
{
    net::url url_("u-s.er:pi.-ass@web-proxy.abc.com:8080/abc/?a=1&b=2#name");
    REQUIRE(url_.protocol() == "http");
    REQUIRE(url_.username() == "u-s.er");
    REQUIRE(url_.password() == "pi.-ass");
    REQUIRE(url_.host() == "web-proxy.abc.com");
    REQUIRE(url_.port() == 8080);
    REQUIRE(url_.path() == "/abc/");
    REQUIRE(url_.param() == "");
    REQUIRE(url_.query() == "a=1&b=2");
    REQUIRE(url_.fragment() == "name");
    REQUIRE(url_.file() == "/abc/?a=1&b=2");
    REQUIRE(url_.str() == "http://u-s.er:pi.-ass@web-proxy.abc.com:8080/abc/?a=1&b=2#name");
}
