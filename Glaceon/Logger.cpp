#include "Logger.h"

namespace Glaceon {

std::shared_ptr<spdlog::logger> Logger::p_console_logger;

void Logger::InitLoggers() {
  p_console_logger = spdlog::stdout_color_mt("GLACEON");
  // set minimum level to show to stdout
  // trace, info, warn, error, critical -> lowest to highest level
  p_console_logger->set_level(spdlog::level::trace);
}
}  // namespace Glaceon
