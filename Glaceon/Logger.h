//
// Created by alyxc on 2/29/2024.
//

#ifndef GLACEON_GLACEON_LOGGER_H_
#define GLACEON_GLACEON_LOGGER_H_

#include <spdlog/spdlog.h>

namespace Glaceon {

class Logger {
 public:
  static void InitLoggers();

  static inline std::shared_ptr<spdlog::logger> GetConsoleLogger() {
    return p_console_logger;
  }

 private:
  static std::shared_ptr<spdlog::logger> p_console_logger;
};

} // glaceon

// Logger macros
#define GLACEON_LOG_TRACE(...) Glaceon::Logger::GetConsoleLogger()->trace(__VA_ARGS__)
#define GLACEON_LOG_INFO(...) Glaceon::Logger::GetConsoleLogger()->info(__VA_ARGS__)
#define GLACEON_LOG_WARN(...) Glaceon::Logger::GetConsoleLogger()->warn(__VA_ARGS__)
#define GLACEON_LOG_ERROR(...) Glaceon::Logger::GetConsoleLogger()->error(__VA_ARGS__)
#define GLACEON_LOG_CRITICAL(...) Glaceon::Logger::GetConsoleLogger()->critical(__VA_ARGS__)

#endif //GLACEON_GLACEON_LOGGER_H_
