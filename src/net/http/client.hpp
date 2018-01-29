#ifndef _NET_HTTP_CLIENT_HPP
#define _NET_HTTP_CLIENT_HPP

#include <cst/lnx/socket.hpp>
#include "net/proxy.hpp"
#include "net/http/error.hpp"
#include "net/http/headers.hpp"
#include "net/http/request.hpp"
#include "net/http/response.hpp"

namespace net {
namespace http {

class client {
public:
    void set_proxy(const char* http_proxy, const char* https_proxy);

    void set_keepalive(bool alive) noexcept
    { keepalive_ = alive; }

    void set_check_cert(bool check) noexcept
    { check_cert_ = check; }

    const http::headers& headers() const noexcept { return headers_; }
    http::headers& headers() noexcept { return headers_; }

    response execute(request& req);

private:
    std::shared_ptr<cst::lnx::socket> connect(const request& req);
    response send(const std::shared_ptr<cst::lnx::socket>& sock, const request& req);

    bool proxied(bool ssl) const noexcept
    { return (http_proxy_ && ! ssl) || (https_proxy_ && ssl); }

private:
    proxy http_proxy_;
    proxy https_proxy_;
    http::headers headers_;
    bool keepalive_ = true;
    bool check_cert_ = true;
    bool auto_redirect_ = true;
};

}
}

#endif
