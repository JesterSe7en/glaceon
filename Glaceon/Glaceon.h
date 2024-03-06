#ifndef GLACEON_GLACEON_GLACEON_H_
#define GLACEON_GLACEON_GLACEON_H_

#include <string>

#include "Base.h"
#include "VulkanRenderer/VulkanContext.h"

namespace Glaceon {

struct GLACEON_API ApplicationInfo {
  std::string name;
};

class GLACEON_API Application {
 public:
  Application(ApplicationInfo* info);
  virtual void onStart() = 0;
  virtual void onUpdate() = 0;
  virtual void onShutdown() = 0;

  VulkanContext& GetVulkanContext() { return context; }

 private:
  VulkanContext context;
};

void GLACEON_API runGame(Application* app);

}  // namespace Glaceon
#endif  // GLACEON_GLACEON_GLACEON_H_
