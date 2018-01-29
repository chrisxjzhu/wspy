#ifndef _WSPY_UTIL_STRINGS_HPP
#define _WSPY_UTIL_STRINGS_HPP

#include <cctype>
#include <cstring>
#include <string>
#include <algorithm>

namespace util {

inline bool str_istarts_with(const std::string& s1,
                             const std::string& s2) noexcept
{
    return ::strncasecmp(s1.c_str(), s2.c_str(), s2.length()) == 0;
}

inline bool str_iequals(const std::string& s1, const std::string& s2) noexcept
{
    return ::strcasecmp(s1.c_str(), s2.c_str()) == 0;
}

inline void str_toupper(char* s) noexcept
{
    for (; *s; ++s)
        if (std::islower(*s))
            *s &= ~0x20;
}

inline void str_tolower(char* s) noexcept
{
    for (; *s; ++s)
        if (std::isupper(*s))
            *s |= 0x20;
}

inline std::string str_toupper_copy(const std::string& s)
{
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(), ::toupper);
    return r;
}

inline std::string str_tolower_copy(const std::string& s)
{
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(), ::tolower);
    return r;
}

inline bool is_crlf(char c) noexcept
{
    return (c == '\r' || c == '\n');
}

inline void str_chomp(char* s) noexcept
{
    std::size_t len = std::strlen(s);

    while (len > 0 && is_crlf(s[len-1]))
        --len;

    s[len] = '\0';
}

inline void str_chomp(std::string& s)
{
    while (!s.empty() && is_crlf(s.back()))
        s.pop_back();
}

inline void str_trim_left(std::string& s)
{
    auto pos = s.find_first_not_of(" \t\r\n");
    if (pos != 0)
        s.erase(0, pos);
}

inline void str_trim_right(std::string& s)
{
    while (!s.empty() && std::isspace(s.back()))
        s.pop_back();
}

/*
 * return 0 if size too small, otherwise the number of chars written into buf,
 * not counting the last '\0'
 */
inline std::size_t str_replace(const char* str, const char* hay,
                               const char* ndl, char* buf,
                               std::size_t size, unsigned times) noexcept
{
    std::size_t res = 0;
    const char* pt;

    std::size_t len1 = std::strlen(hay);
    std::size_t len2 = std::strlen(ndl);

    while (times-- && (pt = std::strstr(str, hay))) {
        std::size_t len0 = pt - str;

        res += len0 + len2;
        if (res + 1 > size)
            return 0;

        std::strncpy(buf, str, len0);
        buf += len0;
        std::strcpy(buf, ndl);
        buf += len2;

        str = pt + len1;
    }

    res += std::strlen(str);
    if (res + 1 > size)
        return 0;

    std::strcpy(buf, str);

    return res;
}

inline std::string str_replace_copy(const std::string& str,
                                    const std::string& hay,
                                    const std::string& ndl,
                                    unsigned times)
{
    if (times == 0)
        return str;

    std::string res;

    for (std::size_t pos0 = 0; times; --times) {
        auto pos1 = str.find(hay, pos0);
        if (pos1 == std::string::npos) {
            res.append(str, pos0, std::string::npos);
            break;
        }
        res.append(str, pos0, pos1 - pos0);
        res.append(ndl);
        pos0 = pos1 + hay.length();
    }

    return res;
}

inline std::string str_replace_all_copy(const std::string& str,
                                        const std::string& hay,
                                        const std::string& ndl)
{
    return str_replace_copy(str, hay, ndl, -1);
}

struct str_lower_hasher {
    size_t operator()(const std::string& name) const
    {
        return std::hash<std::string>()(str_tolower_copy(name));
    }
};

struct str_iequality {
    bool operator()(const std::string& name1, const std::string& name2) const
    {
        return str_iequals(name1, name2);
    }
};

}

#endif
