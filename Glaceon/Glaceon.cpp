#include "Glaceon.h"
#include "pch.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <cstdio>

namespace Glaceon {

void error_callback(int error, const char *description) {
  fprintf(stderr, "Error %d: %s\n", error, description);
}

void GLACEON_API runGame(Application *app) {
  if (!app) {
    fprintf(stdout, "Application is null\n");
    return;
  }

  fprintf(stdout, "Checking for vulkan support...\n");

  if (!glfwInit()) {
    fprintf(stdout, "GLFW initialization failed\n");
    return;
  }

  bool vulkanSupported = glfwVulkanSupported();
  if (vulkanSupported) {
    fprintf(stdout, "Vulkan supported\n");
  } else {
    fprintf(stdout, "Vulkan not supported\n");
  }


  glfwSetErrorCallback(error_callback);

  auto pfnCreateInstance = (PFN_vkCreateInstance)glfwGetInstanceProcAddress(
      nullptr, "vkCreateInstance");

  if (pfnCreateInstance == nullptr) {
    fprintf(stdout, "vkCreateInstance not found\n");
    return;
  } else {
    fprintf(stdout, "vkCreateInstance found: %p\n", pfnCreateInstance);
  }

  GLFWwindow *window =
      glfwCreateWindow(800, 600, "GLFW Test Window", nullptr, nullptr);

  if (!window) {
    fprintf(stdout, "GLFW window creation failed\n");
    return;
  } else {
    fprintf(stdout, "GLFW window created\n");
  }

  app->onStart();
  app->onUpdate();

  glfwDestroyWindow(window);
  glfwTerminate();
  app->onShutdown();
}

} // namespace Glaceon
