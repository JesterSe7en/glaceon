#include "Glaceon.h"
#include "Logger.h"
#include "pch.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

namespace Glaceon {

void error_callback(int error, const char *description) {
  GLACEON_LOG_ERROR("GLFW Error: {}", description);
}

Application::Application() { Logger::InitLoggers(); }

void GLACEON_API runGame(Application *app) {
  if (!app) {
    GLACEON_LOG_TRACE("Application is null");
    return;
  }

  if (!glfwInit()) {
    GLACEON_LOG_TRACE("GLFW initialization failed");
    return;
  }

  bool vulkanSupported = glfwVulkanSupported();
  if (vulkanSupported) {
    GLACEON_LOG_TRACE("Vulkan supported");
  } else {
    GLACEON_LOG_TRACE("Vulkan not supported");
    return;
  }

  glfwSetErrorCallback(error_callback);

  auto pfnCreateInstance = (PFN_vkCreateInstance)glfwGetInstanceProcAddress(
      nullptr, "vkCreateInstance");

  // we are using vulkan, don't load in other apis
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  GLFWwindow *window =
      glfwCreateWindow(800, 600, "GLFW Test Window", nullptr, nullptr);

  glfwShowWindow(window);

  uint32_t property_count;
  vkEnumerateInstanceExtensionProperties(nullptr, &property_count, nullptr);

  GLACEON_LOG_TRACE("Extension count: {}", property_count);

  if (!window) {
    GLACEON_LOG_TRACE("GLFW window creation failed");
    return;
  } else {
    GLACEON_LOG_TRACE("GLFW window created successfully");
  }

  app->onStart();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    app->onUpdate();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  app->onShutdown();
}

} // namespace Glaceon
