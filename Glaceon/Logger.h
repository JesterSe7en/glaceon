//
// Created by alyxc on 2/29/2024.
//

#ifndef GLACEON_GLACEON_LOGGER_H_
#define GLACEON_GLACEON_LOGGER_H_

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace Glaceon {

class Logger {
public:
  static void InitLoggers();

  static inline std::shared_ptr<spdlog::logger> &GetConsoleLogger() {
    return p_console_logger;
  }

private:
  static std::shared_ptr<spdlog::logger> p_console_logger;
};

} // namespace Glaceon

// Built-in formatter of spdlog uses github.com/fmtlib/fmt library
// https://hackingcpp.com/cpp/libs/fmt.html
#define GLACEON_LOG_TRACE(fmt_str, ...)                                        \
  Glaceon::Logger::GetConsoleLogger()->trace(                                  \
      fmt::format(fmt_str, ##__VA_ARGS__));
#define GLACEON_LOG_INFO(fmt_str, ...)                                         \
  Glaceon::Logger::GetConsoleLogger()->info(                                   \
      fmt::format(fmt_str, ##__VA_ARGS__));
#define GLACEON_LOG_WARN(fmt_str, ...)                                         \
  Glaceon::Logger::GetConsoleLogger()->warn(                                   \
      fmt::format(fmt_str, ##__VA_ARGS__));
#define GLACEON_LOG_ERROR(fmt_str, ...)                                        \
  Glaceon::Logger::GetConsoleLogger()->error(                                  \
      fmt::format(fmt_str, ##__VA_ARGS__));
#define GLACEON_LOG_CRITICAL(fmt_str, ...)                                     \
  Glaceon::Logger::GetConsoleLogger()->critical(                               \
      fmt::format(fmt_str, ##__VA_ARGS__));

#endif // GLACEON_GLACEON_LOGGER_H_
