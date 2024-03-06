#ifndef GLACEON_SANDBOXAPP_GAME_H_
#define GLACEON_SANDBOXAPP_GAME_H_

#include "Application.h"
#include "Glaceon.h"

class SandBoxApplication : public Glaceon::Application {
 public:
  SandBoxApplication();
  void onStart() override;
  void onUpdate() override;
  void onShutdown() override;

 private:
  Glaceon::ApplicationInfo app_info = {.name = "Sandbox"};
};

#endif  // GLACEON_SANDBOXAPP_GAME_H_
