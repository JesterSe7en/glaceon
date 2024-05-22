#ifndef GLACEON_GLACEON_APPLICATION_H_
#define GLACEON_GLACEON_APPLICATION_H_

#include "Assimp/AssimpImporter.h"
#include "Core/Base.h"
#include "Core/Logger.h"
#include "Core/Memory/MemorySubsystem.h"
#include "Scene.h"
#include "VulkanRenderer/VulkanContext.h"
#include "pch.h"

namespace glaceon {

struct GLACEON_API ApplicationInfo {
  std::string name;
};

class GLACEON_API Application {
 public:
  explicit Application(ApplicationInfo *info);
  virtual void OnStart() = 0;
  virtual void OnUpdate() = 0;
  virtual void OnShutdown() = 0;

  void PushContent(Assimp_ModelData model_data);

  VulkanContext &GetVulkanContext() { return context_; }
  Scene &GetScene() { return scene_; }

 private:
  VulkanContext context_;
  Scene scene_;
  MemorySubsystem memory_subsystem_;
};
}// namespace glaceon

#endif// GLACEON_GLACEON_APPLICATION_H_
