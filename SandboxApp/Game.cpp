#include "Game.h"

int main() {
  SandBoxApplication app;
  Glaceon::runGame(&app);
}
SandBoxApplication::SandBoxApplication() = default;
void SandBoxApplication::onStart() {}
void SandBoxApplication::onUpdate() {}
void SandBoxApplication::onShutdown() {}
