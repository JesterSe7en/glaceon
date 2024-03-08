#ifndef GLACEON_GLACEON_GLACEON_CPP_APPLICATION_H_
#define GLACEON_GLACEON_GLACEON_CPP_APPLICATION_H_

#include "pch.h"

#include "Base.h"
#include "Logger.h"
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
}  // namespace Glaceon

#endif  // GLACEON_GLACEON_GLACEON_CPP_APPLICATION_H_
