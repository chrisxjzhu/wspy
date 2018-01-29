#ifndef _WSPY_NET_HTTP_HEADERS_H
#define _WSPY_NET_HTTP_HEADERS_H

#include "util/strings.hpp"
#include "net/http/error.hpp"
#include <unordered_map>

namespace net {
namespace http {

class headers {
public:
    void put(const std::string& line)
    {
        std::string name, value;
        parse(line, name, value);
        put(name, value);
    }

    void put(const std::string& name, const std::string& value)
    {
        del(name);
        headers_.insert({name, value});
    }

    void put(const headers& other);

    void add(const std::string& line)
    {
        std::string name, value;
        parse(line, name, value);
        add(name, value);
    }

    void add(const std::string& name, const std::string& value)
    {
        headers_.insert({name, value});
    }

    void add(const headers& other)
    {
        for (const auto& h : other.headers_)
            add(h.first, h.second);
    }

    void del(const std::string& name)
    {
        auto rng = headers_.equal_range(name);
        if (rng.first != rng.second)
            headers_.erase(rng.first, rng.second);
    }

    bool has(const std::string& name) const
    {
        return headers_.find(name) != headers_.end();
    }

    std::string get(const std::string& name) const
    {
        auto it = headers_.find(name);
        if (it == headers_.end())
            return "";
        return it->second;
    }

    std::string str() const;

private:
    void parse(const std::string& line, std::string& name, std::string& value);

    std::unordered_multimap<std::string, std::string, util::str_lower_hasher, util::str_iequality> headers_;
};

}
}

#endif
