#include "Game.h"

#include "Application.h"

int main() {
  SandBoxApplication app;
  glaceon::RunGame(&app);
}
SandBoxApplication::SandBoxApplication() : glaceon::Application(&app_info) {

  // we want to somehow pass a list of vec3 to engine as a push constant

}

void SandBoxApplication::OnStart() {}
void SandBoxApplication::OnUpdate() {}
void SandBoxApplication::OnShutdown() {}
