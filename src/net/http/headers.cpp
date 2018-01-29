#include "net/http/headers.hpp"
#include <sstream>

namespace net {
namespace http {

void headers::put(const headers& other)
{
    std::string last;

    for (const auto& h : other.headers_) {
        if (!util::str_iequals(h.first, last)) {
            del(h.first);
            last = h.first;
        }
        add(h.first, h.second);
    }
}

std::string headers::str() const
{
    std::ostringstream hdrs;

    const std::string& host = get("Host");

    /* Host must be the first */
    if (! host.empty())
        hdrs << "Host: " << host << "\r\n";

    for (auto it = headers_.cbegin(); it != headers_.cend(); ++it)
        if (! util::str_iequals(it->first, "Host"))
            hdrs << it->first << ": " << it->second << "\r\n";

    return hdrs.str();
}

void headers::parse(const std::string& line,
                    std::string& name, std::string& value)
{
    size_t pos = line.find(':');
    if (pos == std::string::npos)
        throw bad_header(line);

    name = line.substr(0, pos);
    util::str_trim_right(name);

    pos = line.find_first_not_of(" \t", pos + 1);
    if (pos != std::string::npos) {
        value = line.substr(pos);
        util::str_trim_right(value);
    } else
        value.clear();
}

}
}
