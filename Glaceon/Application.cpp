#include "Application.h"

namespace glaceon {
Application::Application([[maybe_unused]] ApplicationInfo *info) { glaceon::Logger::InitLoggers(); }
void Application::PushContent(Assimp_ModelData model_data) { scene_ = Scene(model_data); }
}// namespace glaceon
