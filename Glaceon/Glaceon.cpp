#include "Glaceon.h"

#include "Logger.h"
#include "VulkanAPI.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "pch.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Glaceon {

void error_callback(int error, const char *description) { GLACEON_LOG_ERROR("GLFW Error: {}", description); }

Application::Application() { Logger::InitLoggers(); }

static void check_vk_result(VkResult err) {
  if (err == 0) return;
  GLACEON_LOG_ERROR("[vulkan] Error: VkResult = {}", err);
  if (err < 0) abort();
}

void SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd, VkSurfaceKHR surface, int width, int height) {
  wd->Surface = surface;

  VkBool32 res;
  vkGetPhysicalDeviceSurfaceSupportKHR(VulkanAPI::getVulkanPhysicalDevice(),
                                       VulkanAPI::getVulkanGraphicsQueueFamilyIndex(), wd->Surface, &res);
  if (res != VK_TRUE) {
    GLACEON_LOG_ERROR("Error no WSI support on physical device 0");
    exit(-1);
  }

  // Select Surface Format
  const VkFormat requestSurfaceImageFormat[] = {VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM,
                                                VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
  const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
  wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(
      VulkanAPI::getVulkanPhysicalDevice(), wd->Surface, requestSurfaceImageFormat,
      (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);
  VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_FIFO_KHR};

  wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(VulkanAPI::getVulkanPhysicalDevice(), wd->Surface,
                                                        &present_modes[0], IM_ARRAYSIZE(present_modes));
  // printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

  // Create SwapChain, RenderPass, Framebuffer, etc.
  /* IM_ASSERT(g_MinImageCount >= 2); */
  ImGui_ImplVulkanH_CreateOrResizeWindow(VulkanAPI::getVulkanInstance(), VulkanAPI::getVulkanPhysicalDevice(),
                                         VulkanAPI::getVulkanDevice(), wd,
                                         VulkanAPI::getVulkanGraphicsQueueFamilyIndex(), nullptr, width, height, 2);
}

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
  ImGui_ImplVulkanH_Window *imgui_window = nullptr;

  SetupVulkanWindow(imgui_window, surface, w, h);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  auto physicalDevice = VulkanAPI::getVulkanPhysicalDevice();
  auto device = VulkanAPI::getVulkanDevice();
  auto queueFamily = VulkanAPI::getVulkanGraphicsQueueFamilyIndex();
  auto queue = VulkanAPI::getVulkanQueue();
  auto pipelineCache = VulkanAPI::getVulkanPipelineCache();
  auto descriptorPool = VulkanAPI::getVulkanDescriptorPool();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForVulkan(glfw_window, true);
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = instance;
  init_info.PhysicalDevice = physicalDevice;
  init_info.Device = device;
  init_info.QueueFamily = queueFamily;
  init_info.Queue = queue;
  init_info.PipelineCache = pipelineCache;
  init_info.DescriptorPool = descriptorPool;
  init_info.RenderPass = imgui_window->RenderPass;
  init_info.Subpass = 0;
  init_info.MinImageCount = 2;
  init_info.ImageCount = imgui_window->ImageCount;
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  init_info.Allocator = nullptr;
  init_info.CheckVkResultFn = check_vk_result;
  ImGui_ImplVulkan_Init(&init_info);

  // TODO: Create vkSurfaceKHR and pass to glfwCreateWindowSurface

  // TODO: Create framebuffers with glfwGetFramebufferSize()

  // TODO: Setup Imgui context

  app->onStart();

  while (!glfwWindowShouldClose(glfw_window)) {
    glfwPollEvents();
    app->onUpdate();

    // Resize swap chain?
    /* if (g_SwapChainRebuild) { */
    /*   int width, height; */
    /*   glfwGetFramebufferSize(glfw_window, &width, &height); */
    /*   if (width > 0 && height > 0) { */
    /*     ImGui_ImplVulkan_SetMinImageCount(2); */
    /*     ImGui_ImplVulkanH_CreateOrResizeWindow(instance, physicalDevice, device, imgui_window, queueFamily, nullptr,
     * w, */
    /*                                            h, 2); */
    /*     g_MainWindowData.FrameIndex = 0; */
    /*     g_SwapChainRebuild = false; */
    /*   } */
    /* } */

    // Start the Dear ImGui frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    bool showDemo = true;
    ImGui::ShowDemoWindow(&showDemo);
  }

  glfwDestroyWindow(glfw_window);
  glfwTerminate();
  app->onShutdown();
}

}  // namespace Glaceon
