#ifndef GLACEON_SANDBOXAPP_GAME_H_
#define GLACEON_SANDBOXAPP_GAME_H_

#include "Glaceon.h"

class SandBoxApplication : public Glaceon::Application {
 public:
  SandBoxApplication();
  void onStart() override;
  void onUpdate() override;
  void onShutdown() override;
};

#endif //GLACEON_SANDBOXAPP_GAME_H_
