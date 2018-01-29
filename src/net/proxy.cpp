#include "net/proxy.hpp"
#include <regex>

namespace net {

const std::regex re("(\\w+://)?"
                    "(?:([\\w\\-\\.]+)(?::([^@]*))?@)?"
                    "([\\w\\-\\.]+)(?::(\\d{1,5}))?/?");

proxy::proxy(enum type type, const std::string& proxy)
    : type_(type)
{
    std::smatch matches;

    if (! std::regex_match(proxy, matches, re))
        throw proxy_error(proxy);

    if (matches[5].length() == 0)
        throw proxy_no_port(proxy);

    username_ = matches[2];
    password_ = matches[3];
    host_ = matches[4];

    int port = std::stoi(matches[5]);
    if (port <= 0 || port > 0xFFFF)
        throw proxy_bad_port(proxy, port);

    port_ = port;
}

}
