#include "Game.h"

#include "Application.h"

int main() {
  SandBoxApplication app;
  glaceon::RunGame(&app);
}
SandBoxApplication::SandBoxApplication() : glaceon::Application(&app_info) {}

void SandBoxApplication::OnStart() {}
void SandBoxApplication::OnUpdate() {}
void SandBoxApplication::OnShutdown() {}
