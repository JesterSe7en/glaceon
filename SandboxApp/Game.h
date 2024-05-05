#ifndef GLACEON_SANDBOXAPP_GAME_H_
#define GLACEON_SANDBOXAPP_GAME_H_

#include "Application.h"
#include "Glaceon.h"

class SandBoxApplication : public glaceon::Application {
 public:
  SandBoxApplication();
  void OnStart() override;
  void OnUpdate() override;
  void OnShutdown() override;

 private:
  glaceon::ApplicationInfo app_info = {.name = "Sandbox"};
};

#endif// GLACEON_SANDBOXAPP_GAME_H_
