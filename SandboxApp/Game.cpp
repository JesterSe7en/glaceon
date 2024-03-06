#include "Game.h"

#include "Application.h"

int main() {
  SandBoxApplication app;
  Glaceon::runGame(&app);
}
SandBoxApplication::SandBoxApplication() : Glaceon::Application(&app_info) {}

void SandBoxApplication::onStart() {}
void SandBoxApplication::onUpdate() {}
void SandBoxApplication::onShutdown() {}
