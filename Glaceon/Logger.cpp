#include "Logger.h"

namespace glaceon {

std::shared_ptr<spdlog::logger> Logger::p_console_logger_;

void Logger::InitLoggers() {
  p_console_logger_ = spdlog::stdout_color_mt("GLACEON");
  // set minimum level to show to stdout
  // trace, info, warn, error, critical -> lowest to highest level
  p_console_logger_->set_level(spdlog::level::trace);
}
}// namespace glaceon
