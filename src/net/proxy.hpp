#ifndef _WSPY_NET_PROXY_HPP
#define _WSPY_NET_PROXY_HPP

#include <stdexcept>

namespace net {

class proxy {
public:
    enum type { none, http };

    proxy() = default;
    proxy(enum type type, const std::string& proxy);

    enum type type() const noexcept { return type_; }
    const std::string& username() const noexcept { return username_; }
    const std::string& password() const noexcept { return password_; }
    const std::string& host() const noexcept { return host_; }
    uint16_t port() const noexcept { return port_; }

    explicit operator bool() const noexcept { return type_ != none; }

private:
    enum type type_ = none;
    std::string username_;
    std::string password_;
    std::string host_;
    uint16_t port_ = 0;
};

class proxy_error : public std::logic_error {
public:
    explicit proxy_error(const std::string& proxy)
        : logic_error("")
    {
        message_ = "bad proxy '" + proxy + "'";
    }

    const char* what() const noexcept override
    { return message_.c_str(); }

protected:
    std::string message_;
};

class proxy_no_port : public proxy_error {
public:
    explicit proxy_no_port(const std::string& proxy)
        : proxy_error(proxy)
    {
        message_ += ", missing port";
    }
};

class proxy_bad_port : public proxy_error {
public:
    proxy_bad_port(const std::string& proxy, int port)
        : proxy_error(proxy)
    {
        message_ += ", bad port number: " + std::to_string(port);
    }
};

}

#endif
