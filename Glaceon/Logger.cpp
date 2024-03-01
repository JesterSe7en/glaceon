//
// Created by alyxc on 2/29/2024.
//

#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Glaceon {

std::shared_ptr<spdlog::logger> Logger::p_console_logger;

void Logger::InitLoggers() {
  p_console_logger = spdlog::stdout_color_mt("console");
  // set minimum level to show to stdout
  // trace, info, warn, error, critical -> lowest to highest level
  p_console_logger->set_level(spdlog::level::trace);
}
} // namespace Glaceon
