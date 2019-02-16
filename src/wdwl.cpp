#include <cst/program_options.hpp>
#include <cst/progress/bar.hpp>
#include <cst/progress/simple_view.hpp>
#include <cst/logging/logger/trivial_loggers.hpp>

#include <iostream>
#include <cstdio>

#include "util/files.hpp"
#include "net/url.hpp"
#include "net/http/client.hpp"

namespace po = cst::program_options;
namespace logging = cst::logging;

po::options_description desc("wdwl");
po::variables_map vm;

static void wspy_get_options(int argc, const char* argv[])
{
    auto& log_ = logger;
    auto& opt_ = opt;

    desc.add_options()
      (",k", "not check ssl certificate")
      ("no-proxy", "disable proxy")
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

    client.set_check_cert(!vm.count("k"));

    client.headers().put("User-Agent", "wspy/0.1");
    client.headers().put("Accept", "*/*");

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
