#ifndef _WSPY_NET_HTTP_REQUEST_HPP
#define _WSPY_NET_HTTP_REQUEST_HPP

#include "net/url.hpp"
#include "net/http/headers.hpp"
#include <fstream>
#include <sstream>

namespace net {
namespace http {

class request {
public:
    request(const std::string& method, const std::string& path,
            const std::string& host, uint16_t port, bool ssl = false)
        : method_(method), path_(path), host_(host), port_(port), ssl_(ssl)
    {
        put_host_header();
    }

    request(const std::string& method, const url& url)
        : method_(method)
    {
        url_ = url.str();
        path_ = url.file();
        host_ = url.host();
        port_ = url.port();
        ssl_ = (url.scheme().type() == scheme::https);

        put_host_header();
    }

    void put_connection_header(bool keepalive, bool proxied);

    void set_body(const std::string& content)
    {
        body_ = content;
    }

    void set_body_from_file(const std::string& file)
    {
        std::ifstream in(file, std::ios::in | std::ios::binary);
        std::ostringstream oss;
        oss << in.rdbuf();
        body_ = oss.str();
    }

    const std::string& method() const noexcept
    { return method_; }

    const std::string& path(bool proxied) const noexcept
    { return (!url_.empty() && proxied && !ssl_) ? url_ : path_; }

    const std::string& host() const noexcept
    { return host_; }

    uint16_t port() const noexcept
    { return port_; }

    bool ssl() const noexcept
    { return ssl_; }

    std::string start_line(bool proxied) const
    {
        std::ostringstream line;
        line << method_ << ' ' << path(proxied) << ' ' << version_ << "\r\n";
        return line.str();
    }

    const class headers& headers() const noexcept { return headers_; }
    http::headers& headers() noexcept { return headers_; }

    const std::string& body() const noexcept { return body_; }

private:
    void put_host_header();

private:
    const std::string version_ = "HTTP/1.1";
    std::string method_;
    std::string url_;
    std::string path_;
    std::string host_;
    uint16_t port_ = 0;
    bool ssl_ = false;
    http::headers headers_;
    std::string body_;
};

}
}

#endif
