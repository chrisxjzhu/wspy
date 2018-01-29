#ifndef _WSPY_NET_HTTP_ERROR_H
#define _WSPY_NET_HTTP_ERROR_H

#include <stdexcept>

namespace net {
namespace http {

class error : public std::runtime_error {
public:
    explicit error(const std::string& message)
        : runtime_error(""), message_(message)
    { }

    const char* what() const noexcept override
    { return message_.c_str(); }

protected:
    std::string message_;
};

class bad_version : public error {
public:
    bad_version(const std::string& ver)
        : error("bad http version: " + ver)
    { }
};

class bad_header : public error {
public:
    bad_header(const std::string& line)
        : error("bad http header: " + line)
    { }
};

}
}

#endif
