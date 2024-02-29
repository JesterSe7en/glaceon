#ifndef GLACEON_GLACEON_GLACEON_H_
#define GLACEON_GLACEON_GLACEON_H_

#include "Base.h"

namespace Glaceon {

class GLACEON_API Application {
 public:
  Application();
  virtual void onStart() = 0;
  virtual void onUpdate() = 0;
  virtual void onShutdown() = 0;
};

void GLACEON_API runGame(Application *app);

} // namespace Glaceon
#endif // GLACEON_GLACEON_GLACEON_H_