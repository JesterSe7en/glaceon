#include "Application.h"

namespace glaceon {
Application::Application([[maybe_unused]] ApplicationInfo *info) {
  glaceon::Logger::InitLoggers();
  scene_ = Scene();
}
}// namespace glaceon
