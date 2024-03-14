#include "Glaceon.h"

#include "Application.h"
#include "Logger.h"

namespace Glaceon {

static bool swapChainRebuild = false;

void error_callback(int error, const char *description) { GERROR("GLFW Error: Code: {} - {}", error, description); }

void keyboard_callback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
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

static void ImGuiFrameRender(VulkanContext &context, ImDrawData *draw_data) {
  VkResult err;

  std::vector<VkSemaphore> image_available_semaphores = context.GetVulkanSync().GetImageAvailableSemaphores();
  std::vector<VkSemaphore> render_complete_semaphores = context.GetVulkanSync().GetRenderFinishedSemaphores();
  VkDevice device = context.GetVulkanDevice().GetLogicalDevice();
  assert(device != VK_NULL_HANDLE);
  VkSwapchainKHR swapChain = context.GetVulkanSwapChain().GetVkSwapChain();
  assert(swapChain != VK_NULL_HANDLE);

  std::vector<SwapChainFrame> swapChainFrames = context.GetVulkanSwapChain().GetSwapChainFrames();
  VkFence fence = context.GetVulkanSync().GetInFlightFence();
  VkCommandPool commandPool = context.GetVulkanCommandPool().GetVkCommandPool();
  VkCommandBuffer commandBuffer = *(context.GetVulkanCommandPool().GetMainCommandBuffer());
  uint32_t semaphoreIndex = context.semaphoreIndex;

  // This defines the frame index to render to?
  uint32_t wdFrameIndex = context.GetCurrentFrameIndex();
  err = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, image_available_semaphores[semaphoreIndex], VK_NULL_HANDLE,
                              &wdFrameIndex);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
    swapChainRebuild = true;
    return;
  }
  CheckVkResult(err);

  {
    err = vkWaitForFences(device, 1, &fence, VK_TRUE,
                          UINT64_MAX);  // wait indefinitely instead of periodically checking
    CheckVkResult(err);

    err = vkResetFences(device, 1, &fence);
    CheckVkResult(err);
  }
  {
    err = vkResetCommandPool(device, commandPool, 0);
    CheckVkResult(err);
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    err = vkBeginCommandBuffer(commandBuffer, &info);
    CheckVkResult(err);
  }
  {
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = context.GetVulkanRenderPass().GetVkRenderPass();
    info.framebuffer = context.GetVulkanSwapChain().GetSwapChainFrameBuffers()[wdFrameIndex];
    info.renderArea.extent = context.GetVulkanSwapChain().GetSwapChainExtent();
    info.clearValueCount = 1;
    VkClearValue clear_color = {1.0f, 0.0f, 0.0f, 1.0f};
    info.pClearValues = &clear_color;
    vkCmdBeginRenderPass(commandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
  }

  // Record dear imgui primitives into command buffer
  ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer);

  // Submit command buffer
  vkCmdEndRenderPass(commandBuffer);
  {
    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &image_available_semaphores[semaphoreIndex];
    info.pWaitDstStageMask = &wait_stage;
    info.commandBufferCount = 1;
    info.pCommandBuffers = &commandBuffer;
    info.signalSemaphoreCount = 1;
    info.pSignalSemaphores = &render_complete_semaphores[semaphoreIndex];

    err = vkEndCommandBuffer(commandBuffer);
    CheckVkResult(err);
    err = vkQueueSubmit(context.GetVulkanDevice().GetPresentQueue(), 1, &info, fence);
    CheckVkResult(err);
  }
}

static void ImGuiFramePresent(VulkanContext &context) {
  if (swapChainRebuild) return;
  std::vector<VkSemaphore> render_complete_semaphores = context.GetVulkanSync().GetRenderFinishedSemaphores();
  uint32_t wdFrameIndex = context.GetCurrentFrameIndex();
  std::vector<VkSwapchainKHR> swapChains;
  swapChains.push_back(context.GetVulkanSwapChain().GetVkSwapChain());

  VkPresentInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  info.waitSemaphoreCount = 1;
  info.pWaitSemaphores = &render_complete_semaphores[context.semaphoreIndex];
  info.swapchainCount = 1;
  info.pSwapchains = swapChains.data();
  info.pImageIndices = &wdFrameIndex;
  VkResult err = vkQueuePresentKHR(context.GetVulkanDevice().GetPresentQueue(), &info);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
    swapChainRebuild = true;
    return;
  }
  CheckVkResult(err);
  context.semaphoreIndex =
      (context.semaphoreIndex + 1) % context.GetVulkanSwapChain()
                                         .GetSwapChainFrames()
                                         .size();  // mod semaphore index to wrap indexing back to beginning
}

// ----------------- Application Class Functions ----------------------

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

  VulkanContext &context = app->GetVulkanContext();
  glfwSetErrorCallback(error_callback);

  // we are using vulkan, don't load in other apis
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  GLFWwindow *glfw_window = glfwCreateWindow(800, 600, "GLFW Test Window", nullptr, nullptr);

  glfwSetKeyCallback(glfw_window, keyboard_callback);

  // For reference on integrating ImGui with GLFW and Vulkan
  // https://github.com/ocornut/imgui/blob/master/examples/example_glfw_vulkan/main.cpp
  // check required extensions for glfw
  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
  if (!glfw_extensions) {
    GTRACE("GLFW extension check failed");
    return;
  }
  for (uint32_t i = 0; i < glfw_extension_count; i++) {
    context.GetInstanceExtensions().push_back(glfw_extensions[i]);
  }

  context.GetVulkanBackend().Initialize();
  VkInstance instance = context.GetVulkanInstance();
  if (instance == VK_NULL_HANDLE) {
    GERROR("Failed to create Vulkan instance");
    return;
  }
  VkSurfaceKHR surface;
  VkResult res = glfwCreateWindowSurface(instance, glfw_window, nullptr, &surface);
  if (res != VK_SUCCESS) {
    GERROR("Failed to create window surface");
  }
  context.SetSurface(surface);
  context.AddDeviceExtension("VK_KHR_swapchain");
  context.GetVulkanDevice().Initialize();
  context.GetVulkanRenderPass().Initialize();
  context.GetVulkanSwapChain().Initialize();
  context.GetVulkanCommandPool().Initialize();
  context.GetVulkanSync().Initialize();

  GraphicsPipelineConfig config = {
      .vertexShaderFile = "../../shaders/vert.spv",
      .fragmentShaderFile = "../../shaders/frag.spv",
  };
  context.GetVulkanPipeline().Initialize(config);

  // create frame buffers for each of the swap chain images.

  // Setup Dear ImGui
  int w, h;
  glfwGetFramebufferSize(glfw_window, &w, &h);

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

  ImGui_ImplGlfw_InitForVulkan(glfw_window, true);

  // Setup Platform/Renderer backends
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = context.GetVulkanInstance();
  init_info.PhysicalDevice = context.GetVulkanPhysicalDevice();
  init_info.Device = context.GetVulkanLogicalDevice();
  init_info.QueueFamily = context.GetQueueIndexes().graphicsFamily.value();
  init_info.Queue = context.GetVulkanDevice().GetGraphicsQueue();
  init_info.PipelineCache = VK_NULL_HANDLE;
  init_info.DescriptorPool = context.GetDescriptorPool();
  init_info.RenderPass = context.GetVulkanRenderPass().GetVkRenderPass();
  init_info.Subpass = 0;
  init_info.MinImageCount = 2;
  init_info.ImageCount = context.GetVulkanSwapChain().GetSwapChainFrames().size();
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  init_info.Allocator = nullptr;
  init_info.CheckVkResultFn = CheckVkResult;
  bool success = ImGui_ImplVulkan_Init(&init_info);
  if (!success) {
    GERROR("Failed to initialize ImGui")
    return;
  }

  ImGui_ImplVulkan_CreateFontsTexture();

  GINFO("ImGui successfully initialized")

  app->onStart();
  while (!glfwWindowShouldClose(glfw_window)) {
    glfwPollEvents();
    app->onUpdate();

    if (swapChainRebuild) {
      GINFO("Rebuilding swapchain...");
      int width, height;
      glfwGetFramebufferSize(glfw_window, &width, &height);
      if (width > 0 && height > 0) {
        context.GetVulkanSwapChain().RebuildSwapChain(width, height);
        context.ResetCurrentFrameIndex();
        swapChainRebuild = false;
      }
    }

    // Start the Dear ImGui frame
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
      // bool showDemo = true;
      // ImGui::ShowDemoWindow(&showDemo);

      ImGui::Begin("FPS");
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      // for multi-port support
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
    }

    ImDrawData *draw_data = ImGui::GetDrawData();
    const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
    if (!is_minimized) {
      ImGuiFrameRender(context, draw_data);
      ImGuiFramePresent(context);
    }
  }

  res = vkDeviceWaitIdle(context.GetVulkanDevice().GetLogicalDevice());

  if (res != VK_SUCCESS) {
    GERROR("Failed to wait for device");
    exit(-1);
  }

  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  vkDestroyDescriptorPool(context.GetVulkanLogicalDevice(), context.GetDescriptorPool(), nullptr);

  vkDestroyDevice(context.GetVulkanLogicalDevice(), nullptr);
  vkDestroyInstance(instance, nullptr);

  glfwDestroyWindow(glfw_window);
  glfwTerminate();
  app->onShutdown();
}

}  // namespace Glaceon
