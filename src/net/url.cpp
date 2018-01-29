#include "net/url.hpp"
#include "util/strings.hpp"
#include <regex>
#include <vector>

namespace net {

const std::vector<scheme> schemes = {
    scheme(scheme::none,  "", 0),
    scheme(scheme::http,  "http", 80),
    scheme(scheme::https, "https", 443)
};

const scheme& scheme::get(enum type type) noexcept
{
    return schemes[type];
}

const scheme& scheme::get(const std::string& name) noexcept
{
    for (const auto& scheme : schemes)
        if (util::str_iequals(scheme.name(), name))
            return scheme;

    return schemes[scheme::none];
}

const std::regex re("(?:(\\w+)://)?"
                    "(?:([\\w\\-\\.]+)(?::([^@]*))?@)?"
                    "([\\w\\-\\.]+)"
                    "(?::(\\d{1,5}))?"
                    "(/[^;^?^#]*)?"
                    "(?:;([^?^#]*))?"
                    "(?:\\?([^#]*))?"
                    "(?:#([\\w\\-\\.]*))?");

url::url(const std::string& url)
{
    std::smatch matches;

    if (! std::regex_match(url, matches, re))
        throw url_error(url);

    scheme_ = scheme::get(matches[1] != "" ? matches[1].str() : "http");
    if (scheme_.type() == scheme::none)
        throw url_bad_scheme(url, matches[1].str());

    username_ = matches[2];
    password_ = matches[3];
    host_ = matches[4];

    int port = (matches[5] != "" ? std::stoi(matches[5]) : scheme_.default_port());
    if (port <= 0 || port > 0xFFFF)
        throw url_bad_port(url, port);

    port_ = port;

    path_ = (matches[6] != "" ? matches[6].str() : "/");
    param_ = matches[7];
    query_ = matches[8];
    fragment_ = matches[9];
}

std::string url::file() const
{
    std::string file = path_;

    if (! param_.empty()) {
        file.push_back(';');
        file.append(param_);
    }

    if (! query_.empty()) {
        file.push_back('?');
        file.append(query_);
    }

    return file;
}

std::string url::str() const
{
    std::string ustr;

    ustr.append(protocol()).append("://");

    if (! username_.empty()) {
        ustr.append(username_);

        if (! password_.empty()) {
            ustr.push_back(':');
            ustr.append(password_);
        }

        ustr.push_back('@');
    }

    ustr.append(host_);

    if (port_ != scheme_.default_port()) {
        ustr.push_back(':');
        ustr.append(std::to_string(port_));
    }

    ustr.append(file());

    if (! fragment_.empty()) {
        ustr.push_back('#');
        ustr.append(fragment_);
    }

    return ustr;
}

std::istream& operator>>(std::istream& is, url& u)
{
    std::string str;

    if (is >> str)
        u = url(str);

    return is;
}

std::ostream& operator<<(std::ostream& os, const url& u)
{
    return os << u.str();
}

}
