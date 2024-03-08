#ifndef GLACEON_GLACEON_LOGGER_H_
#define GLACEON_GLACEON_LOGGER_H_

#include "pch.h"

#ifdef _DEBUG
#define DEBUG_LOG_ENABLED 1
#define TRACE_LOG_ENABLED 1
#else
#define DEBUG_LOG_ENABLED 0
#define TRACE_LOG_ENABLED 0
#endif

namespace Glaceon {

class Logger {
 public:
  static void InitLoggers();
  static inline std::shared_ptr<spdlog::logger> &GetConsoleLogger() { return p_console_logger; }

 private:
  static std::shared_ptr<spdlog::logger> p_console_logger;
};

}  // namespace Glaceon

// Built-in formatter of spdlog uses github.com/fmtlib/fmt library
// https://hackingcpp.com/cpp/libs/fmt.html
#if (DEBUG_LOG_ENABLED)
#define GDEBUG(fmt_str, ...) Glaceon::Logger::GetConsoleLogger()->debug(fmt::format(fmt_str, ##__VA_ARGS__));
#else
#define GDEBUG(fmt_str, ...)
#endif

#if (TRACE_LOG_ENABLED)
#define GTRACE(fmt_str, ...) Glaceon::Logger::GetConsoleLogger()->trace(fmt::format(fmt_str, ##__VA_ARGS__));
#else
#define GTRACE(fmt_str, ...)
#endif

#define GINFO(fmt_str, ...) Glaceon::Logger::GetConsoleLogger()->info(fmt::format(fmt_str, ##__VA_ARGS__));
#define GWARN(fmt_str, ...) Glaceon::Logger::GetConsoleLogger()->warn(fmt::format(fmt_str, ##__VA_ARGS__));
#define GERROR(fmt_str, ...) Glaceon::Logger::GetConsoleLogger()->error(fmt::format(fmt_str, ##__VA_ARGS__));
#define GCRITICAL(fmt_str, ...) \
  Glaceon::Logger::GetConsoleLogger()->critical(fmt::format(fmt_str, ##__VA_ARGS__));

#endif  // GLACEON_GLACEON_LOGGER_H_
