#ifndef _WSPY_NET_HTTP_RESPONSE_HPP
#define _WSPY_NET_HTTP_RESPONSE_HPP

#include <cst/lnx/socket.hpp>
#include "net/http/error.hpp"
#include "net/http/headers.hpp"

namespace net {
namespace http {

class response {
public:
    enum {
        MULTIPLE_CHOICES = 300,
        MOVED_PERMANENTLY = 301,
        FOUND = 302,
        SEE_OTHER = 303,
        NOT_MODIFIED = 304,
        USE_PROXY = 305,
        TEMPORARY_REDIRECT = 307
    };

    response() = default;

    response(const std::shared_ptr<cst::lnx::socket>& sock);

    bool success() const noexcept
    { return (status_ >= 200 && status_ < 300); }

    bool redirect() const noexcept
    {
        return (status_ == MOVED_PERMANENTLY ||
                status_ == FOUND ||
                status_ == SEE_OTHER ||
                status_ == TEMPORARY_REDIRECT);
    }

    const http::headers& headers() const noexcept { return headers_; }

    std::string head() const;

    std::size_t read(void* buf, std::size_t len);

private:
    void read_line(std::string& line);
    bool read_header(std::string& name, std::string& value);
    void read_headers();
    void read_chunksize(std::size_t& size);

private:
    std::shared_ptr<cst::lnx::socket> socket_;

    int status_ = 0;
    std::string version_;
    std::string phrase_;
    http::headers headers_;

    struct header {
        bool chunked = false;
        std::size_t length = 0;
    } header_;

    struct read_ctrl {
        bool  over = false;
        std::size_t sum = 0;
        std::size_t rcs = 0;
    } rcb_;
};

}
}

#endif
