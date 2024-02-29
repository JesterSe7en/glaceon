#include "Glaceon.h"
#include "pch.h"
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdio>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Glaceon {

void error_callback(int error, const char *description) {
  fprintf(stderr, "Error %d: %s\n", error, description);
}

void GLACEON_API runGame(Application *app) {
  auto console = spdlog::stdout_color_mt("console");
  spdlog::get("console")->info("SPDLOG: running game...");
  if (!app) {
    fprintf(stdout, "Application is null\n");
    return;
  }

  if (!glfwInit()) {
    fprintf(stdout, "GLFW initialization failed; bailing...\n");
    return;
  }

  bool vulkanSupported = glfwVulkanSupported();
  if (vulkanSupported) {
    fprintf(stdout, "Vulkan supported\n");
  } else {
    fprintf(stdout, "Vulkan not supported; bailing...\n");
    return;
  }

  glfwSetErrorCallback(error_callback);

  auto pfnCreateInstance = (PFN_vkCreateInstance) glfwGetInstanceProcAddress(
      nullptr, "vkCreateInstance");

  // we are using vulkan, don't load in other apis
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  GLFWwindow *window =
      glfwCreateWindow(800, 600, "GLFW Test Window", nullptr, nullptr);

  glfwShowWindow(window);

  uint32_t property_count;
  vkEnumerateInstanceExtensionProperties(nullptr, &property_count, nullptr);

  fprintf(stdout, "Extension count: %d\n", property_count);

  if (!window) {
    fprintf(stdout, "GLFW window creation failed\n");
    return;
  } else {
    fprintf(stdout, "GLFW window created\n");
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
