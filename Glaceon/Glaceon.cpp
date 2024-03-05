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
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Glaceon {

static bool swapChainRebuild = false;
static ImGui_ImplVulkanH_Window g_MainWindowData;

void error_callback(int error, const char *description) { GERROR("GLFW Error: Code: {} - {}", error, description); }

void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    GINFO("Escape key pressed, closing window...");
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

static void CheckVkResult(VkResult err) {
  if (err == 0) return;
  GERROR("[Vulkan] Error: VkResult = {}", string_VkResult(err));
  if (err < 0) abort();
}

void SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd, VkSurfaceKHR surface, int width, int height) {
  if (wd == nullptr) {
    GERROR("Failed to create Vulkan Window");
    exit(-1);
  }
  wd->Surface = surface;

  VkBool32 res;
  vkGetPhysicalDeviceSurfaceSupportKHR(VulkanAPI::getVulkanPhysicalDevice(),
                                       VulkanAPI::getVulkanGraphicsQueueFamilyIndex(), wd->Surface, &res);
  if (res != VK_TRUE) {
    GERROR("Error no WSI support on physical device 0");
    exit(-1);
  }

  // Select Surface Format
  const VkFormat requestSurfaceImageFormat[] = {VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM,
                                                VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
  const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
  wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(
      VulkanAPI::getVulkanPhysicalDevice(), wd->Surface, requestSurfaceImageFormat,
      (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

  GINFO("SurfaceFormat = {}", string_VkFormat(wd->SurfaceFormat.format));

  // uncapped fps - if uncapped present modes are not supported, default to vsync
  // present_modes[] is a priority list with idx 0 being the highest
  VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR,
                                      VK_PRESENT_MODE_FIFO_KHR};

  wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(VulkanAPI::getVulkanPhysicalDevice(), wd->Surface,
                                                        &present_modes[0], IM_ARRAYSIZE(present_modes));

  GINFO("PresentMode = {}", string_VkPresentModeKHR(wd->PresentMode));

  // Create SwapChain, RenderPass, Framebuffer, etc.
  /* IM_ASSERT(g_MinImageCount >= 2); */
  ImGui_ImplVulkanH_CreateOrResizeWindow(VulkanAPI::getVulkanInstance(), VulkanAPI::getVulkanPhysicalDevice(),
                                         VulkanAPI::getVulkanDevice(), wd,
                                         VulkanAPI::getVulkanGraphicsQueueFamilyIndex(), nullptr, width, height, 2);
}

static void FrameRender(ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data) {
  VkResult err;

  VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
  VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
  auto device = VulkanAPI::getVulkanDevice();

  err = vkAcquireNextImageKHR(device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE,
                              &wd->FrameIndex);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
    swapChainRebuild = true;
    return;
  }
  CheckVkResult(err);

  ImGui_ImplVulkanH_Frame *fd = &wd->Frames[wd->FrameIndex];
  {
    err = vkWaitForFences(device, 1, &fd->Fence, VK_TRUE,
                          UINT64_MAX);  // wait indefinitely instead of periodically checking
    CheckVkResult(err);

    err = vkResetFences(device, 1, &fd->Fence);
    CheckVkResult(err);
  }
  {
    err = vkResetCommandPool(device, fd->CommandPool, 0);
    CheckVkResult(err);
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
    CheckVkResult(err);
  }
  {
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = wd->RenderPass;
    info.framebuffer = fd->Framebuffer;
    info.renderArea.extent.width = wd->Width;
    info.renderArea.extent.height = wd->Height;
    info.clearValueCount = 1;
    info.pClearValues = &wd->ClearValue;
    vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
  }

  // Record dear imgui primitives into command buffer
  ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

  // Submit command buffer
  vkCmdEndRenderPass(fd->CommandBuffer);
  {
    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &image_acquired_semaphore;
    info.pWaitDstStageMask = &wait_stage;
    info.commandBufferCount = 1;
    info.pCommandBuffers = &fd->CommandBuffer;
    info.signalSemaphoreCount = 1;
    info.pSignalSemaphores = &render_complete_semaphore;

    err = vkEndCommandBuffer(fd->CommandBuffer);
    CheckVkResult(err);
    err = vkQueueSubmit(VulkanAPI::getVulkanQueue(), 1, &info, fd->Fence);
    CheckVkResult(err);
  }
}

static void FramePresent(ImGui_ImplVulkanH_Window *wd) {
  if (swapChainRebuild) return;
  VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
  VkPresentInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  info.waitSemaphoreCount = 1;
  info.pWaitSemaphores = &render_complete_semaphore;
  info.swapchainCount = 1;
  info.pSwapchains = &wd->Swapchain;
  info.pImageIndices = &wd->FrameIndex;
  VkResult err = vkQueuePresentKHR(VulkanAPI::getVulkanQueue(), &info);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
    swapChainRebuild = true;
    return;
  }
  CheckVkResult(err);
  wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->SemaphoreCount;  // Now we can use the next set of semaphores
}

// ----------------- Application Class Functions ----------------------

Application::Application(ApplicationInfo *info) {
  VulkanAPI::defineVulkanApp(info);
  Logger::InitLoggers();
}

void GLACEON_API runGame(Application *app) {
  if (!app) {
    GTRACE("Application is null");
    return;
  }

  if (!glfwInit()) {
    GTRACE("GLFW initialization failed");
    return;
  }

  bool vulkanSupported = glfwVulkanSupported();
  if (vulkanSupported) {
    GTRACE("Vulkan supported");
  } else {
    GTRACE("Vulkan not supported");
    exit(-1);
  }

  glfwSetErrorCallback(error_callback);

  // we are using vulkan, don't load in other apis
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  GLFWwindow *glfw_window = glfwCreateWindow(800, 600, "GLFW Test Window", nullptr, nullptr);

  glfwSetKeyCallback(glfw_window, keyboard_callback);

  // For reference on integrating ImGui with GLFW and Vulkan
  // https://github.com/ocornut/imgui/blob/master/examples/example_glfw_vulkan/main.cpp
  // check required extensions for glfw
  std::vector<const char *> extensions;
  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
  if (!glfw_extensions) {
    GTRACE("GLFW extension check failed");
    return;
  }
  for (uint32_t i = 0; i < glfw_extension_count; i++) {
    app->GetVulkanContext().AddInstanceExtension(glfw_extensions[i]);
  }

  /* app->GetVulkanContext().GetVulkanBackend().Initialize(); */
  //  app->GetVulkanContext().GetVulkanDevice().Initialize();

  VulkanAPI::initVulkan(extensions);

  VkSurfaceKHR surface;
  VkInstance instance = VulkanAPI::getVulkanInstance();
  if (instance == VK_NULL_HANDLE) {
    GERROR("Failed to create Vulkan instance");
    return;
  }

  VkResult res = glfwCreateWindowSurface(VulkanAPI::getVulkanInstance(), glfw_window, nullptr, &surface);
  if (res != VK_SUCCESS) {
    GERROR("Failed to create window surface");
  }

  int w, h;
  glfwGetFramebufferSize(glfw_window, &w, &h);
  ImGui_ImplVulkanH_Window *imgui_window = &g_MainWindowData;

  // TODO: Maybe return something indicating success or failure
  // init_info_renderPass = imgui_window->RenderPass; // This can be null
  SetupVulkanWindow(imgui_window, surface, w, h);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Enable Multi-Viewport

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
  init_info.CheckVkResultFn = CheckVkResult;
  ImGui_ImplVulkan_Init(&init_info);

  app->onStart();

  ImVec4 clear_color = ImVec4(1.0f, 0.0f, 0.0f, 1.00f);

  while (!glfwWindowShouldClose(glfw_window)) {
    glfwPollEvents();
    app->onUpdate();

    if (swapChainRebuild) {
      GINFO("Rebuilding swapchain");
      int width, height;
      glfwGetFramebufferSize(glfw_window, &width, &height);
      if (width > 0 && height > 0) {
        ImGui_ImplVulkan_SetMinImageCount(2);
        ImGui_ImplVulkanH_CreateOrResizeWindow(instance, physicalDevice, device, imgui_window, queueFamily, nullptr, w,
                                               h, 2);
        g_MainWindowData.FrameIndex = 0;
        swapChainRebuild = false;
      }
    }

    // Start the Dear ImGui frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // bool showDemo = true;
    // ImGui::ShowDemoWindow(&showDemo);

    {
      ImGui::Begin("FPS");
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    // for multi-port support
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    ImDrawData *draw_data = ImGui::GetDrawData();
    const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
    if (!is_minimized) {
      imgui_window->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
      imgui_window->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
      imgui_window->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
      imgui_window->ClearValue.color.float32[3] = clear_color.w;
      FrameRender(imgui_window, draw_data);
      FramePresent(imgui_window);
    }
  }

  res = vkDeviceWaitIdle(VulkanAPI::getVulkanDevice());

  if (res != VK_SUCCESS) {
    GERROR("Failed to wait for device");
    exit(-1);
  }

  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  ImGui_ImplVulkanH_DestroyWindow(instance, device, imgui_window, nullptr);
  vkDestroyDescriptorPool(device, descriptorPool, nullptr);

  vkDestroyDevice(device, nullptr);
  vkDestroyInstance(instance, nullptr);

  glfwDestroyWindow(glfw_window);
  glfwTerminate();
  app->onShutdown();
}

}  // namespace Glaceon
