#include <cst/program_options.hpp>
#include <cst/progress/bar.hpp>
#include <cst/progress/simple_view.hpp>
#include <cst/logging/logger/trivial_loggers.hpp>

#include <iostream>
#include <cstdio>

#include "util/files.hpp"
#include "util/base64.hpp"
#include "net/url.hpp"
#include "net/http/client.hpp"

namespace po = cst::program_options;
namespace logging = cst::logging;

po::options_description desc("wspy");
po::variables_map vm;

auto logger = logging::trivial_stderr_logger("wspy");

struct wspy_option {
    std::string method;
    net::http::headers custom_headers;
    net::url url;
    std::string cred_b64;
} opt;

static void wspy_get_options(int argc, const char* argv[])
{
    auto& log_ = logger;
    auto& opt_ = opt;

    desc.add_options()
      (",X", po::value<std::string>(&opt.method)->default_value("GET"), "http method", "method")
      (",H", po::value<std::vector<std::string>>()->notifier(
        [&opt_](const std::vector<std::string>& hdrs) {
          for (const auto& h : hdrs)
            opt_.custom_headers.add(h);
        }), "add extra header", "header")
      (",u", po::value<std::string>()->notifier(
        [&opt_](const std::string& cred) {
          opt_.cred_b64 = util::base64_encode(cred);
        }), "specify username and password", "user:password")
      (",k", "not check ssl certificate")
      ("keep-alive", po::value<bool>()->default_value(false)->implicit_value(true), "enable keep-alive")
      ("no-proxy", "disable proxy")
      ("data", po::value<std::string>(), "payload data", "string or @filename")
      ("save,s", po::value<std::string>()->implicit_value(""), "save file", "filename")
      (",v", po::value<void>()->notifier(
          [&log_]() { log_->set_level(logging::level::info); }
        ), "print verbose info")
      ("dbg", po::value<void>()->notifier(
          [&log_]() { log_->set_level(logging::level::debug); }
        ), "print debug info")
      ("help,h", "print help text")
      ("url", po::value<net::url>(&opt.url)->required(), "", "", true);

    po::positional_options_description pos_desc;
    pos_desc.add("url", 1);

    try {
        po::store(parse_command_line(argc, argv, desc, pos_desc), vm);

        if (vm.count("help")) {
            std::cerr << desc << std::endl;
            std::exit(0);
        }

        po::notify(vm);

    } catch (const po::option_error& oe) {
        LOG_ERROR(logger) << oe.what();
        std::exit(1);
    } catch (const net::url_error& ue) {
        LOG_ERROR(logger) << ue.what();
        std::exit(1);
    }

    if (opt.cred_b64.empty()) {
        if (! opt.url.username().empty())
            opt.cred_b64 = util::base64_encode(opt.url.username() + ':' + opt.url.password());
    }
}

int main(int argc, const char* argv[])
{
    wspy_get_options(argc, argv);

    net::http::client client;
    net::http::request req(opt.method, opt.url);
    net::http::response resp;

    if (!vm.count("no-proxy")) {
        try {
            client.set_proxy(getenv("http_proxy"), getenv("https_proxy"));
        } catch (const net::proxy_error& pe) {
            LOG_ERROR(logger) << pe.what();
            LOG_WARN(logger) << "ignore the proxy setting";
        }
    }

    client.set_keepalive(vm["keep-alive"].as<bool>());
    client.set_check_cert(!vm.count("k"));

    client.headers().put("User-Agent", "wspy/0.1");
    client.headers().put("Accept", "*/*");
    client.headers().put(opt.custom_headers);

    if (!opt.cred_b64.empty())
        client.headers().put("Authorization", "Basic " + opt.cred_b64);

    if (vm.count("data")) {
        if (client.headers().get("Content-Type").empty())
            req.headers().put("Content-Type", "application/x-www-form-urlencoded");

        const char* data = vm["data"].as<std::string>().c_str();
        if (data[0] == '@')
            req.set_body_from_file(data + 1);
        else
            req.set_body(data);
        req.headers().put("Content-Length", std::to_string(req.body().size()));
    }

    try {
        resp = client.execute(req);
    } catch (const net::http::error& he) {
        LOG_ERROR(logger) << he.what();
        LOG_ERROR(logger) << "Failed to execute request '" << opt.method << ' ' << opt.url.str() << "'";
        std::exit(1);
    }

    if (opt.method == "HEAD")
        std::fwrite(resp.head().c_str(), 1, resp.head().size(), stdout);
    else {
        char buf[4096];
        std::size_t r;

        if (vm.count("save")) {
            std::string filename = vm["save"].as<std::string>();
            if (filename.empty()) {
                filename = util::file_name(opt.url.path());
                if (filename.empty())
                    filename = "index";
                if (filename.find('.') == std::string::npos) {
                    std::string type = resp.headers().get("Content-Type");
                    if (! type.empty()) {
                        auto pos = type.rfind('/');
                        if (pos != std::string::npos)
                            type = type.substr(pos + 1);

                        pos = type.find_first_of("; ");
                        if (pos != std::string::npos)
                            type.erase(pos);

                        filename.push_back('.');
                        filename.append(type);
                    }
                }
            }

            std::unique_ptr<std::FILE, decltype(std::fclose)*>
            savefile(std::fopen(filename.c_str(), "w"), std::fclose);

            std::string length = resp.headers().get("Content-Length");
            if (length.empty())
                length = "0";

            cst::progress::bar bar(std::stoi(length),
                           std::make_shared<cst::progress::simple_view>(40));

            while ((r = resp.read(buf, sizeof(buf))) > 0) {
                std::fwrite(buf, 1, r, savefile.get());
                bar.move(r);
                bar.render();
            }

            bar.finish();
        } else {
            while ((r = resp.read(buf, sizeof(buf))) > 0)
                std::fwrite(buf, 1, r, stdout);
        }
    }

    return 0;
}
