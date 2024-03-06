//
// Created by alexc on 3/6/24.
//

#ifndef GLACEON_GLACEON_GLACEON_CPP_APPLICATION_H_
#define GLACEON_GLACEON_GLACEON_CPP_APPLICATION_H_

#include <string>

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

  Glaceon::VulkanContext& GetVulkanContext() { return context; }

 private:
  Glaceon::VulkanContext context;
};
}  // namespace Glaceon

#endif  // GLACEON_GLACEON_GLACEON_CPP_APPLICATION_H_
