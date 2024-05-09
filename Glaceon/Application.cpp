#include "Application.h"

namespace glaceon {
Application::Application([[maybe_unused]] ApplicationInfo *info) {
  glaceon::Logger::InitLoggers();
  char *test = (char *) MemorySubsystem::GAllocate(10, MemoryTag::MEMORY_TAG_STRING);
  test[0] = 'a';
  test[1] = 'b';
  GTRACE("{}", MemorySubsystem::GetStats());
  MemorySubsystem::GFree(test, 10, MemoryTag::MEMORY_TAG_STRING);
  GTRACE("{}", MemorySubsystem::GetStats());
}
void Application::PushContent(Assimp_ModelData model_data) { scene_ = Scene(model_data); }
}// namespace glaceon
