#include "net/http/client.hpp"
#include <cst/logging/logger.hpp>

extern std::shared_ptr<cst::logging::logger> logger;

namespace net {
namespace http {

void client::set_proxy(const char* http_proxy, const char* https_proxy)
{
    if (http_proxy && *http_proxy)
        http_proxy_ = proxy(proxy::http, http_proxy);

    if (https_proxy && *https_proxy)
        https_proxy_ = proxy(proxy::http, https_proxy);
}

response client::execute(request& req)
{
    req.headers().put(headers_);
    req.put_connection_header(keepalive_, proxied(req.ssl()));

    std::shared_ptr<cst::lnx::socket> sock = connect(req);
    response resp = send(sock, req);

    while (auto_redirect_ && resp.redirect()) {
        const std::string& loc = resp.headers().get("Location");

        request nreq("GET", loc);
        nreq.headers().put(headers_);
        nreq.put_connection_header(keepalive_, proxied(nreq.ssl()));

        sock = connect(nreq);
        resp = send(sock, nreq);
    }

    return resp;
}

std::shared_ptr<cst::lnx::socket> client::connect(const request& req)
{
    std::shared_ptr<cst::lnx::socket> sock = std::make_shared<cst::lnx::socket>(AF_INET, SOCK_STREAM, 0);

    if (https_proxy_ && req.ssl()) {
        sock->connect(https_proxy_.host(), https_proxy_.port());

        std::string arg = req.host() + ":" + std::to_string(req.port());
        request creq("CONNECT", arg, req.host(), req.port(), req.ssl());

        response resp = send(sock, creq);
        if (! resp.success())
           throw error("ssl proxy tunneling error");

    } else if (http_proxy_ && ! req.ssl()) {
        sock->connect(http_proxy_.host(), http_proxy_.port());
    } else {
        sock->connect(req.host(), req.port());
    }

    if (req.ssl()) {
        sock->ssl_connect();

        if (check_cert_ && ! sock->verify_certificate())
            throw error("no verified certificate presented by " + req.host());
    }

    return sock;
}

response client::send(const std::shared_ptr<cst::lnx::socket>& sock, const request& req)
{
    std::string head = req.start_line(proxied(req.ssl()));
    head.append(req.headers().str());
    head.append("\r\n");

    if (logger->filter(cst::logging::level::info)) {
        std::string info = head;
        util::str_trim_right(info);
        info = util::str_replace_all_copy(info, "\r\n", "\r\n> ");
        info.append("\r\n>");
        LOG_INFO(logger) << "Sending request ...\n> " << info;
    }

    sock->send(head.c_str(), head.size());

    if (!req.body().empty())
        sock->send(req.body().c_str(), req.body().size());

    return response(sock);
}

}
}
