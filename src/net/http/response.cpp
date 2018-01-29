#include <cst/logging/logger.hpp>
#include <sstream>
#include "net/http/response.hpp"

extern std::shared_ptr<cst::logging::logger> logger;

namespace net {
namespace http {

response::response(const std::shared_ptr<cst::lnx::socket>& sock)
    : socket_(sock)
{
    std::string line;
    std::istringstream istream;

    read_line(line);

    if (line.empty())
        throw error("empty response");

    istream.str(line);
    istream >> version_ >> status_;

    if (! util::str_istarts_with(version_, "HTTP/1"))
        throw bad_version(version_);

    istream >> phrase_;

    {
        std::string remain;
        std::getline(istream, remain);
        phrase_.append(remain);
    }

    read_headers();

    if (logger->filter(cst::logging::level::info)) {
        std::string info = head();
        util::str_trim_right(info);
        info = util::str_replace_all_copy(info, "\r\n", "\r\n< ");
        info.append("\r\n<");
        LOG_INFO(logger) << "Received response ...\n< " << info;
    }

    const std::string& cl = headers_.get("Content-Length");
    if (! cl.empty())
        header_.length = std::stoi(cl);

    if (util::str_iequals(headers_.get("Transfer-Encoding"), "Chunked"))
        header_.chunked = true;
}

std::string response::head() const
{
    std::ostringstream oss;
    oss << version_ << ' ' << status_ << ' ' << phrase_ << "\r\n"
        << headers_.str() << "\r\n";
    return oss.str();
}

std::size_t response::read(void* buf, std::size_t len)
{
    if (rcb_.over)
        return 0;

    if (header_.chunked) {
        if (rcb_.rcs == 0) {
            read_chunksize(rcb_.rcs);
            if (rcb_.rcs == 0) {
                rcb_.over = true;
                read_headers();
                return 0;
            }
        }
        len = std::min(len, rcb_.rcs);
    } else {
        if (header_.length > 0)
            len = std::min(len, header_.length - rcb_.sum);
    }

    std::string dummy;
    std::size_t r = socket_->recv(buf, len);

    if (r > 0) {
        rcb_.sum += r;
        if (header_.chunked) {
            rcb_.rcs -= r;
            if (rcb_.rcs == 0)
                read_line(dummy);
        } else {
            if (rcb_.sum == header_.length)
                rcb_.over = true;
        }
    } else {
        rcb_.over = true;
    }

    return r;
}

void response::read_line(std::string& line)
{
    char buf[2048];

    line.clear();

    while (true) {
        std::size_t n = socket_->recv_until(buf, sizeof(buf), '\n');
        if (n == 0)
            break;

        line.append(buf, n);

        if (line.back() == '\n')
            break;
    }

    util::str_chomp(line);
}

bool response::read_header(std::string& name, std::string& value)
{
    std::string header, line;
    std::size_t n;
    char pc;

    n = socket_->peek(&pc, 1);

    /* EOF or headers end */
    if (n == 0 || pc == '\r' || pc == '\n')
        return false;

    /* a continued line, should not happen */
    /* allow it
    if (std::isblank(pc)) {
        LOG_ERROR(logger) << "unexpected continued header line";
        return false;
    }
    */

    /* a good line, get it */
    read_line(header);

    util::str_trim_left(header);

    /* check if having continued lines */
    while (true) {
        n = socket_->peek(&pc, 1);

        /* EOF or headers end */
        if (n == 0 || pc == '\r' || pc == '\n')
            break;

        /* not a continued line, stop here */
        if (!std::isblank(pc))
            break;

        /* a continued line, get it */
        read_line(line);

        n = line.find_first_not_of(" \t");
        header.append(line, n, std::string::npos);
    }

    n = header.find(':');
    if (n == std::string::npos) {
        LOG_ERROR(logger) << "malformed header: '" << header << "'";
        return false;
    }

    name = header.substr(0, n);
    util::str_trim_right(name);

    value.clear();
    n = header.find_first_not_of(" \t", n + 1);
    if (n != std::string::npos) {
        value = header.substr(n);
        util::str_trim_right(value);
    }

    return true;
}

void response::read_headers()
{
    std::string name, value;

    while (read_header(name, value))
        headers_.add(name, value);

    /* consumes the trailing "\r\n" */
    read_line(value);
}

void response::read_chunksize(std::size_t& size)
{
    std::string line;
    long len;
    char* ep;

    read_line(line);

    len = ::strtol(line.c_str(), &ep, 16);
    if (len >= 0 && (*ep == ';' || *ep == '\0'))
        size = (std::size_t) len;
    else
        size = 0;
}

}
}
