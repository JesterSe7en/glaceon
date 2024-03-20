#include "Glaceon.h"

#include "Application.h"
#include "GLFW/glfw3.h"
#include "Logger.h"

namespace glaceon {

static bool swapChainRebuild = false;
static Application *currentApp = nullptr;

void ErrorCallback(int error, const char *description) { GERROR("GLFW Error: Code: {} - {}", error, description); }

void CheckVkResult(VkResult result) {
  auto res = static_cast<vk::Result>(result);
  if (res != vk::Result::eSuccess) {
    GERROR("Vulkan error: {}", vk::to_string(res));
    exit(-1);
  }
}

void KeyboardCallback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action,
                      [[maybe_unused]] int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    GINFO("Escape key pressed, closing window...");
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

static void ImGuiFrameRender(VulkanContext &context, ImDrawData *draw_data) {
  std::vector<vk::Semaphore> image_available_semaphores = context.GetVulkanSync().GetImageAvailableSemaphores();
  std::vector<vk::Semaphore> render_complete_semaphores = context.GetVulkanSync().GetRenderFinishedSemaphores();
  vk::Device device = context.GetVulkanLogicalDevice();
  assert(device != VK_NULL_HANDLE);
  vk::SwapchainKHR swap_chain = context.GetVulkanSwapChain().GetVkSwapchain();
  assert(swap_chain != VK_NULL_HANDLE);

  std::vector<SwapChainFrame> swap_chain_frames = context.GetVulkanSwapChain().GetSwapChainFrames();
  std::vector<vk::Fence> fences = context.GetVulkanSync().GetInFlightFences();
  vk::CommandPool command_pool = context.GetVulkanCommandPool().GetVkCommandPool();
  uint32_t semaphore_index = context.semaphore_index_;

  // This defines the frame index to render to?, give me an available image from
  // the swap chain
  vk::Result res = device.acquireNextImageKHR(swap_chain, UINT64_MAX, image_available_semaphores[semaphore_index],
                                              VK_NULL_HANDLE, &context.current_frame_index_);

  vk::CommandBuffer command_buffer =
      context.GetVulkanCommandPool().GetVkFrameCommandBuffers()[context.current_frame_index_];
  if (res == vk::Result::eErrorOutOfDateKHR || res == vk::Result::eSuboptimalKHR) {
    swapChainRebuild = true;
    return;
  }

  {
    vk::Result err = device.waitForFences(1, &fences[context.current_frame_index_], VK_TRUE, UINT64_MAX);
    if (err == vk::Result::eTimeout) {
      GERROR("Fence timeout!");
      return;
    }

    err = device.resetFences(1, &fences[context.current_frame_index_]);
    if (err != vk::Result::eSuccess) {
      GERROR("Failed to reset fences!");
      return;
    }
  }
  {
    device.resetCommandPool(command_pool);
    vk::CommandBufferBeginInfo info = {};
    info.sType = vk::StructureType::eCommandBufferBeginInfo;
    info.flags = vk::CommandBufferUsageFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    if (command_buffer.begin(&info) == vk::Result::eSuccess) {
      GERROR("Failed to begin command buffer");
      return;
    }
  }
  {
    vk::RenderPassBeginInfo render_pass_begin_info = {};
    render_pass_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
    render_pass_begin_info.renderPass = context.GetVulkanRenderPass().GetVkRenderPass();
    render_pass_begin_info.framebuffer =
        context.GetVulkanSwapChain().GetSwapChainFrames()[context.current_frame_index_].frame_buffer;
    render_pass_begin_info.renderArea.extent = context.GetVulkanSwapChain().GetSwapChainExtent();
    render_pass_begin_info.clearValueCount = 1;
    vk::ClearValue clear_value = {};
    clear_value.color.float32[0] = 0.0f;
    clear_value.color.float32[1] = 0.0f;
    clear_value.color.float32[2] = 0.0f;
    clear_value.color.float32[3] = 0.0f;
    render_pass_begin_info.pClearValues = &clear_value;
    command_buffer.beginRenderPass(&render_pass_begin_info, vk::SubpassContents::eInline);
  }

  // Record dear imgui primitives into command buffer
  ImGui_ImplVulkan_RenderDrawData(draw_data, command_buffer);

  // Submit command buffer
  command_buffer.endRenderPass();
  {
    auto wait_stage = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    vk::SubmitInfo info = {};
    info.sType = vk::StructureType::eSubmitInfo;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &image_available_semaphores[semaphore_index];
    info.pWaitDstStageMask = &wait_stage;
    info.commandBufferCount = 1;
    info.pCommandBuffers = &command_buffer;
    info.signalSemaphoreCount = 1;
    info.pSignalSemaphores = &render_complete_semaphores[semaphore_index];

    command_buffer.end();
    vk::Queue queue = context.GetVulkanDevice().GetVkPresentQueue();
    if (queue.submit(1, &info, fences[context.current_frame_index_]) != vk::Result::eSuccess) {
      GERROR("Failed to submit draw command buffer");
      return;
    }
  }
}

static void ImGuiFramePresent(VulkanContext &context) {
  if (swapChainRebuild) return;
  std::vector<vk::Semaphore> render_complete_semaphores = context.GetVulkanSync().GetRenderFinishedSemaphores();
  vk::SwapchainKHR swap_chain = context.GetVulkanSwapChain().GetVkSwapchain();
  vk::Queue present_queue = context.GetVulkanDevice().GetVkPresentQueue();

  vk::PresentInfoKHR present_info_khr = {};
  present_info_khr.sType = vk::StructureType::ePresentInfoKHR;// VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info_khr.waitSemaphoreCount = 1;
  present_info_khr.pWaitSemaphores = &render_complete_semaphores[context.semaphore_index_];
  present_info_khr.swapchainCount = 1;
  present_info_khr.pSwapchains = &swap_chain;
  present_info_khr.pImageIndices = &context.current_frame_index_;

  vk::Result res = present_queue.presentKHR(&present_info_khr);
  if (res == vk::Result::eErrorOutOfDateKHR || res == vk::Result::eSuboptimalKHR) {
    swapChainRebuild = true;
    return;
  }
  if (res != vk::Result::eSuccess) {
    GERROR("Failed to present swap chain image");
    return;
  }
  context.semaphore_index_ = (context.semaphore_index_ + 1)
      % context.GetVulkanSwapChain()
            .GetSwapChainFrames()
            .size();// mod semaphore index to wrap indexing back to beginning
}

static void RecordDrawCommands(vk::CommandBuffer command_buffer, uint32_t image_index) {
  VulkanContext &context = currentApp->GetVulkanContext();

  vk::CommandBufferBeginInfo begin_info = {};
  begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
  if (command_buffer.begin(&begin_info) != vk::Result::eSuccess) {
    GERROR("Failed to begin recording command buffer");
    return;
  }

  vk::RenderPassBeginInfo render_pass_info = {};
  render_pass_info.sType = vk::StructureType::eRenderPassBeginInfo;
  render_pass_info.renderPass = context.GetVulkanRenderPass().GetVkRenderPass();
  render_pass_info.framebuffer = context.GetVulkanSwapChain().GetSwapChainFrames()[image_index].frame_buffer;
  render_pass_info.renderArea.offset = vk::Offset2D{0, 0};
  render_pass_info.renderArea.extent = context.GetVulkanSwapChain().GetSwapChainExtent();
  vk::ClearValue clear_value = {};
  clear_value.color.float32[0] = 0.0f;
  clear_value.color.float32[1] = 0.0f;
  clear_value.color.float32[2] = 0.0f;
  clear_value.color.float32[3] = 0.0f;
  render_pass_info.clearValueCount = 1;
  render_pass_info.pClearValues = &clear_value;

  command_buffer.beginRenderPass(&render_pass_info, vk::SubpassContents::eInline);
  vk::Pipeline pipeline = context.GetVulkanPipeline().GetVkPipeline();
  command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

  // command_buffer.pushConstants() this defines push constants for shader code to use
  // command_buffer.pushConstants(layout, stageflags, offset, size, value);
  // layout is the pipeline layout
  // stageflags is the shader stage to push constants (in our case we want to push constants to the vertex shader)
  // offset is the offset in the push constant block
  // size is the size of the data in the push constant block
  vk::PipelineLayout pipeline_layout = context.GetVulkanPipeline().GetVkPipelineLayout();
//  glm::mat4 model_matrix =
//      glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));// Move the triangle down halfway
//
//  command_buffer.pushConstants(pipeline_layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), &model_matrix);

  // take a look at scene class and use the triangle_positions_ vector

  std::vector<glm::vec3> triangle_positions = currentApp->GetScene().triangle_positions_;

  for (auto position : triangle_positions) {
    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), position);
    command_buffer.pushConstants(pipeline_layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), &model_matrix);
    command_buffer.draw(3, 1, 0, 0);// This draws a triangle - hard coded for now
  }

  command_buffer.endRenderPass();
  command_buffer.end();
}

static void GameFrameRender(VulkanContext &context) {
  std::vector<vk::Fence> in_flight_fences = context.GetVulkanSync().GetInFlightFences();
  vk::Device device = context.GetVulkanLogicalDevice();
  vk::SwapchainKHR swap_chain = context.GetVulkanSwapChain().GetVkSwapchain();
  vk::Queue graphics_queue = context.GetVulkanDevice().GetVkGraphicsQueue();
  std::vector<vk::Semaphore> image_available_semaphores = context.GetVulkanSync().GetImageAvailableSemaphores();
  std::vector<vk::Semaphore> render_complete_semaphores = context.GetVulkanSync().GetRenderFinishedSemaphores();

  (void) device.waitForFences(1, &in_flight_fences[context.current_frame_index_], VK_TRUE, UINT64_MAX);
  // get image from swap chain
  // the semaphore passes is what is going to be signaled once the image is
  // acquired

  vk::Result res =
      device.acquireNextImageKHR(swap_chain, UINT64_MAX, image_available_semaphores[context.semaphore_index_],
                                 VK_NULL_HANDLE, &context.current_frame_index_);

  // reset the fence - "close the fence behind us"
  (void) device.resetFences(1, &in_flight_fences[context.current_frame_index_]);

  if (res == vk::Result::eErrorOutOfDateKHR || res == vk::Result::eSuboptimalKHR) {
    swapChainRebuild = true;
    return;
  }

  // get the frame's own command buffer
  std::vector<vk::CommandBuffer> frame_command_buffers = context.GetVulkanCommandPool().GetVkFrameCommandBuffers();
  vk::CommandBuffer command_buffer = frame_command_buffers[context.current_frame_index_];
  command_buffer.reset();
  RecordDrawCommands(command_buffer, context.current_frame_index_);

  // submit the command buffer
  auto wait_stage = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
  vk::SubmitInfo submit_info = {};
  submit_info.sType = vk::StructureType::eSubmitInfo;
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = &image_available_semaphores[context.semaphore_index_];
  submit_info.pWaitDstStageMask = &wait_stage;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer;
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &render_complete_semaphores[context.semaphore_index_];

  // fence is provided here so that once we submit the command buffer, we can
  // safely reset the fence
  if (graphics_queue.submit(1, &submit_info, in_flight_fences[context.current_frame_index_]) != vk::Result::eSuccess) {
    GERROR("Failed to submit to queue");
    return;
  }
}

static void GameFramePresent(VulkanContext &context) {
  std::vector<vk::Semaphore> render_complete_semaphores = context.GetVulkanSync().GetRenderFinishedSemaphores();
  vk::SwapchainKHR swap_chain = context.GetVulkanSwapChain().GetVkSwapchain();
  vk::Queue present_queue = context.GetVulkanDevice().GetVkPresentQueue();

  // submit command buffer aka present
  vk::PresentInfoKHR present_info = {};
  present_info.sType = vk::StructureType::ePresentInfoKHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &render_complete_semaphores[context.semaphore_index_];
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &swap_chain;
  present_info.pImageIndices = &context.current_frame_index_;

  vk::Result res = present_queue.presentKHR(&present_info);
  if (res == vk::Result::eErrorOutOfDateKHR || res == vk::Result::eSuboptimalKHR) {
    swapChainRebuild = true;
    return;
  }

  if (res != vk::Result::eSuccess) {
    GERROR("Failed to present swap chain image");
    return;
  }

  context.semaphore_index_ = (context.semaphore_index_ + 1)
      % context.GetVulkanSwapChain()
            .GetSwapChainFrames()
            .size();// mod semaphore index to wrap indexing back to beginning
}

// void framebufferResizeCallback(GLFWwindow *window, int width, int height) {
//   GTRACE("WHOA, framebuffer resize callback");
// }

// ----------------- Application Class Functions ----------------------

void GLACEON_API RunGame(Application *app) {
  if (!app) {
    GTRACE("Application is null");
    return;
  }

  currentApp = app;

  if (!glfwInit()) {
    GTRACE("GLFW initialization failed");
    return;
  }

  bool vulkan_supported = glfwVulkanSupported();
  if (vulkan_supported) {
    GTRACE("Vulkan supported");
  } else {
    GTRACE("Vulkan not supported");
    exit(-1);
  }

  VulkanContext &context = app->GetVulkanContext();
  glfwSetErrorCallback(ErrorCallback);

  // we are using vulkan, don't load in other apis
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  GLFWwindow *glfw_window = glfwCreateWindow(800, 600, "GLFW Test Window", nullptr, nullptr);

  if (glfw_window == nullptr) {
    GERROR("Failed to create GLFW window");

    const char *description;
    glfwGetError(&description);
    GERROR("Cannot initialize GLFW: {}", description);
    glfwTerminate();
    return;
  }

  glfwSetKeyCallback(glfw_window, KeyboardCallback);

  // For reference on integrating ImGui with GLFW and Vulkan
  // https://github.com/ocornut/imgui/blob/master/examples/example_glfw_vulkan/main.cpp
  // check required extensions for glfw
  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
  if (!glfw_extensions) {
    GTRACE("GLFW extension check failed");
    return;
  }
  for (uint32_t i = 0; i < glfw_extension_count; i++) { context.GetInstanceExtensions().push_back(glfw_extensions[i]); }

  context.GetVulkanBackend().Initialize();
  vk::Instance instance = context.GetVulkanInstance();
  if (instance == VK_NULL_HANDLE) {
    GERROR("Failed to create Vulkan instance");
    return;
  }

  VkSurfaceKHR surface;
  glfwCreateWindowSurface(static_cast<VkInstance>(instance), glfw_window, nullptr, &surface);

  // TODO: set up resize callback instead of using a flag
  // glfwSetFramebufferSizeCallback(glfw_window, framebufferResizeCallback);

  context.SetSurface(surface);
  context.AddDeviceExtension(vk::KHRSwapchainExtensionName);
  context.GetVulkanDevice().Initialize();
  context.GetVulkanRenderPass().Initialize();
  context.GetVulkanSwapChain().Initialize();
  context.GetVulkanCommandPool().Initialize();
  context.GetVulkanSync().Initialize();

  GraphicsPipelineConfig config = {
      .vertex_shader_file = "../../shaders/vert.spv",
      .fragment_shader_file = "../../shaders/frag.spv",
  };
  context.GetVulkanPipeline().Initialize(config);

  // Setup Dear ImGui
  int w, h;
  glfwGetFramebufferSize(glfw_window, &w, &h);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void) io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForVulkan(glfw_window, true);

  // Setup Platform/Renderer backends
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = context.GetVulkanInstance();
  init_info.PhysicalDevice = context.GetVulkanPhysicalDevice();
  init_info.Device = context.GetVulkanLogicalDevice();
  init_info.QueueFamily = context.GetQueueIndexes().graphics_family.value();
  init_info.Queue = context.GetVulkanDevice().GetVkGraphicsQueue();
  init_info.PipelineCache = VK_NULL_HANDLE;
  init_info.DescriptorPool = context.GetDescriptorPool();
  init_info.RenderPass = context.GetVulkanRenderPass().GetVkRenderPass();
  init_info.Subpass = 0;
  init_info.MinImageCount = 2;
  init_info.ImageCount = static_cast<uint32_t>(context.GetVulkanSwapChain().GetSwapChainFrames().size());
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  init_info.Allocator = nullptr;
  init_info.CheckVkResultFn = CheckVkResult;
  bool success = ImGui_ImplVulkan_Init(&init_info);
  if (!success) {
    GERROR("Failed to initialize ImGui");
    return;
  }

  ImGui_ImplVulkan_CreateFontsTexture();

  GINFO("ImGui successfully initialized");

  app->OnStart();
  // ----------------------------- MAIN LOOP ----------------------------- //
  while (!glfwWindowShouldClose(glfw_window)) {
    glfwPollEvents();
    app->OnUpdate();

    if (swapChainRebuild) {
      int width, height;
      glfwGetFramebufferSize(glfw_window, &width, &height);

      if (width > 0 && height > 0) {
        GINFO("Rebuilding swapchain... width: {}, height: {}", width, height);

        context.GetVulkanLogicalDevice().waitIdle();
        // destroy pipeline and old sync objects
        context.GetVulkanRenderPass().Destroy();
        context.GetVulkanRenderPass().Initialize();
        // context.GetVulkanPipeline().Destroy();
        context.GetVulkanSwapChain().RebuildSwapChain(width, height);
        context.GetVulkanCommandPool().ResetCommandPool();
        context.GetVulkanCommandPool().RebuildCommandBuffers();
        context.GetVulkanSync().Destroy();
        context.GetVulkanSync().Initialize();
        context.current_frame_index_ = 0;
        swapChainRebuild = false;
      }
    }

    // ------------------ Render Game Frame ------------------ //
    // Essentially we are doing the same thing as ImGuiRender and ImGuiPresent
    // Just that we are rendering the game frames
    GameFrameRender(context);
    GameFramePresent(context);

    // #if _DEBUG
    //     // ------------------ Render ImGui Frame ------------------ //
    //     // Start the Dear ImGui frame
    //     ImGui_ImplGlfw_NewFrame();
    //     ImGui::NewFrame();
    //
    //     {
    //       // bool showDemo = true;
    //       // ImGui::ShowDemoWindow(&showDemo);
    //
    //       ImGui::Begin("FPS");
    //       ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f
    //       / io.Framerate, io.Framerate); ImGui::End();
    //     }
    //
    //     // Rendering
    //     ImGui::Render();
    //     if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    //       // for multi-port support
    //       ImGui::UpdatePlatformWindows();
    //       ImGui::RenderPlatformWindowsDefault();
    //     }
    //
    //     ImDrawData *draw_data = ImGui::GetDrawData();
    //     const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f ||
    //     draw_data->DisplaySize.y <= 0.0f); if (!is_minimized) {
    //       ImGuiFrameRender(context, draw_data);
    //       ImGuiFramePresent(context);
    //     }
    // #endif
  }

  context.GetVulkanLogicalDevice().waitIdle();

  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  context.Destroy();

  glfwDestroyWindow(glfw_window);
  glfwTerminate();
  app->OnShutdown();
}

}// namespace glaceon
