#ifndef _NET_HTTP_DOWNLOADER_HPP
#define _NET_HTTP_DOWNLOADER_HPP

#include <cst/lnx/socket.hpp>
#include "net/proxy.hpp"
#include "net/http/error.hpp"
#include "net/http/headers.hpp"
#include "net/http/request.hpp"
#include "net/http/response.hpp"

namespace net {
namespace http {

class downloader {
public:
    void set_proxy(const char* http_proxy, const char* https_proxy);

    void download(const std::string& url);

private:
    std::shared_ptr<cst::lnx::socket> connect(const request& req);
    response send(const std::shared_ptr<cst::lnx::socket>& sock, const request& req);

    bool proxied(bool ssl) const noexcept
    { return (http_proxy_ && ! ssl) || (https_proxy_ && ssl); }

private:
    proxy http_proxy_;
    proxy https_proxy_;
};

}
}

#endif
