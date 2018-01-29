#include <fstream>

namespace util {

inline std::size_t file_size(const std::string& path)
{
    std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

inline std::string file_name(const std::string& path)
{
    std::string fn = path;

    while (! fn.empty() && fn.back() == '/')
        fn.pop_back();

    auto pos = fn.rfind('/');
    if (pos != std::string::npos)
        fn = fn.substr(pos + 1);

    return fn;
}

}
