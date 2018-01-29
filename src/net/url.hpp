#ifndef _WSPY_NET_URL_HPP
#define _WSPY_NET_URL_HPP

#include <stdexcept>

namespace net {

class scheme {
public:
    enum type { none, http, https };

    static const scheme& get(enum type type) noexcept;
    static const scheme& get(const std::string& name) noexcept;

    scheme() = default;
    scheme(enum type type, const std::string& name, uint16_t port)
        : type_(type), name_(name), default_port_(port)
    { }

    enum type type() const noexcept
    { return type_; }

    const std::string& name() const noexcept
    { return name_; }

    uint16_t default_port() const noexcept
    { return default_port_; }

private:
    enum type type_ = none;
    std::string name_;
    uint16_t default_port_ = 0;
};

/* <scheme>://<user>:<pass>@<host>:<port>/<path>;<param>?<query>#<fragment> */

class url {
    friend std::istream& operator>>(std::istream& is, url& u);
    friend std::ostream& operator<<(std::ostream& os, const url& u);

public:
    url() = default;
    url(const std::string& url);

    const class scheme& scheme() const noexcept { return scheme_; }
    const std::string& protocol() const noexcept { return scheme_.name(); }
    const std::string& username() const noexcept { return username_; }
    const std::string& password() const noexcept { return password_; }
    const std::string& host() const noexcept { return host_; }
    uint16_t port() const noexcept { return port_; }
    const std::string& path() const noexcept { return path_; }
    const std::string& param() const noexcept { return param_; }
    const std::string& query() const noexcept { return query_; }
    const std::string& fragment() const noexcept { return fragment_; }

    std::string file() const;
    std::string str() const;

private:
    class scheme scheme_;
    std::string username_;
    std::string password_;
    std::string host_;
    uint16_t port_ = 0;
    std::string path_;
    std::string param_;
    std::string query_;
    std::string fragment_;
};

class url_error : public std::logic_error {
public:
    explicit url_error(const std::string& url)
        : logic_error("")
    {
        message_ = "bad url '" + url + "'";
    }

    const char* what() const noexcept override
    { return message_.c_str(); }

protected:
    std::string message_;
};

class url_bad_scheme : public url_error {
public:
    url_bad_scheme(const std::string& url, const std::string& scheme)
        : url_error(url)
    {
        message_ += ", unknown scheme: '" + scheme + "'";
    }
};

class url_bad_port : public url_error {
public:
    url_bad_port(const std::string& url, int port)
        : url_error(url)
    {
        message_ += ", bad port number: " + std::to_string(port);
    }
};

}

#endif
