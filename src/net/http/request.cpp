#include "net/http/request.hpp"

namespace net {
namespace http {

void request::put_connection_header(bool keepalive, bool proxied)
{
    if (keepalive) {
        if (proxied)
            headers_.put("Proxy-Connection", "keep-alive");
        else
            headers_.put("Connection", "keep-alive");
    } else
        headers_.put("Connection", "close");
}

void request::put_host_header()
{
    std::ostringstream oss(host_, std::ios_base::ate);

    if ((!ssl_ && port_ != 80) || (ssl_ && port_ != 443))
        oss << ':' << port_;

    headers_.put("Host", oss.str());
}

}
}
