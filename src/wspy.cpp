#include <cst/program_options.hpp>
#include <cst/logging/logger/trivial_logger.hpp>
#include <cst/logging/message/raw_message.hpp>
#include <cst/logging/sink/stderr_sink.hpp>
#include <cst/logging/formatter/simple_formatter.hpp>

namespace po = cst::program_options;
namespace logging = cst::logging;

po::options_description desc("wspy");
po::variables_map vm;

auto loggger = std::make_shared<logging::trivial_logger<logging::raw_message>>(
               "wspy",
               std::make_shared<logging::stderr_sink>(),
               std::make_shared<logging::simple_formatter>());

int main(int argc, const char* argv[])
{
    return 0;
}
