#ifndef GLACEON_GLACEON_LOGGER_H_
#define GLACEON_GLACEON_LOGGER_H_

#include "../pch.h"

#ifdef _DEBUG
#define DEBUG_LOG_ENABLED 1
#define TRACE_LOG_ENABLED 1
#else
#define DEBUG_LOG_ENABLED 0
#define TRACE_LOG_ENABLED 0
#endif

namespace glaceon {

class Logger {
 public:
  static void InitLoggers();
  static inline std::shared_ptr<spdlog::logger> &GetConsoleLogger() { return p_console_logger_; }

 private:
  static std::shared_ptr<spdlog::logger> p_console_logger_;
};

}// namespace glaceon

// Built-in formatter of spdlog uses github.com/fmtlib/fmt library
// https://hackingcpp.com/cpp/libs/fmt.html
//
// C++ 20 standard affords us the capability to use variadic macros with the __VA_OPT__ macro
// https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
#if (DEBUG_LOG_ENABLED)
#define GDEBUG(fmt_str, ...) Glaceon::Logger::GetConsoleLogger()->debug(fmt_str __VA_OPT__(,) __VA_ARGS__)
#else
#define GDEBUG(fmt_str, ...)
#endif

#if (TRACE_LOG_ENABLED)
#define GTRACE(fmt_str, ...) glaceon::Logger::GetConsoleLogger()->trace(fmt_str __VA_OPT__(,) __VA_ARGS__)
#else
#define GTRACE(fmt_str, ...)
#endif

#define GINFO(fmt_str, ...) glaceon::Logger::GetConsoleLogger()->info(fmt_str __VA_OPT__(,) __VA_ARGS__)
#define GWARN(fmt_str, ...) glaceon::Logger::GetConsoleLogger()->warn(fmt_str __VA_OPT__(,) __VA_ARGS__)
#define GERROR(fmt_str, ...) glaceon::Logger::GetConsoleLogger()->error(fmt_str __VA_OPT__(,) __VA_ARGS__)
#define GCRITICAL(fmt_str, ...) Glaceon::Logger::GetConsoleLogger()->critical(fmt_str __VA_OPT__(,) __VA_ARGS__)

#endif// GLACEON_GLACEON_LOGGER_H_
