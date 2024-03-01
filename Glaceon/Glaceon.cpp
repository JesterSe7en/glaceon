#include "Glaceon.h"
#include "Logger.h"
#include "VulkanAPI.h"
#include "pch.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace Glaceon {

void error_callback(int error, const char *description) { GLACEON_LOG_ERROR("GLFW Error: {}", description); }

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

  auto pfnCreateInstance = (PFN_vkCreateInstance)glfwGetInstanceProcAddress(nullptr, "vkCreateInstance");

  // we are using vulkan, don't load in other apis
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  GLFWwindow *glfw_window = glfwCreateWindow(800, 600, "GLFW Test Window", nullptr, nullptr);

  // For reference on integrating ImGui with GLFW and Vulkan
  // https://github.com/ocornut/imgui/blob/master/examples/example_glfw_vulkan/main.cpp
  // check required extensions for glfw
  std::vector<const char *> extensions;
  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
  if (!glfw_extensions) {
    GLACEON_LOG_TRACE("GLFW extension check failed");
    return;
  }
  for (uint32_t i = 0; i < glfw_extension_count; i++) {
    extensions.push_back(glfw_extensions[i]);
  }

  VulkanAPI::initVulkan(extensions);

  VkSurfaceKHR surface;
  VkInstance instance = VulkanAPI::getVulkanInstance();
  if (instance == VK_NULL_HANDLE) {
    GLACEON_LOG_ERROR("Failed to create Vulkan instance");
    return;
  }

  VkResult res = glfwCreateWindowSurface(VulkanAPI::getVulkanInstance(), glfw_window, nullptr, &surface);
  if (res != VK_SUCCESS) {
    GLACEON_LOG_ERROR("Failed to create window surface");
  }

  int w, h;
  glfwGetFramebufferSize(glfw_window, &w, &h);
  ImGui_ImplVulkanH_Window *wd = nullptr;

  // SetupVulkanWindow

  // TODO: Create vkSurfaceKHR and pass to glfwCreateWindowSurface

  // TODO: Create framebuffers with glfwGetFramebufferSize()

  // TODO: Setup Imgui context

  glfwShowWindow(glfw_window);

  uint32_t property_count;
  vkEnumerateInstanceExtensionProperties(nullptr, &property_count, nullptr);

  GLACEON_LOG_TRACE("Extension count: {}", property_count);

  if (!glfw_window) {
    GLACEON_LOG_TRACE("GLFW window creation failed");
    return;
  } else {
    GLACEON_LOG_TRACE("GLFW window created successfully");
  }

  app->onStart();

  while (!glfwWindowShouldClose(glfw_window)) {
    glfwPollEvents();
    app->onUpdate();
  }

  glfwDestroyWindow(glfw_window);
  glfwTerminate();
  app->onShutdown();
}

}  // namespace Glaceon
