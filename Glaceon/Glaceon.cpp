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

static void ImGuiInitialize(VulkanContext &context, GLFWwindow *glfw_window) {
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
  init_info.PipelineCache = context.GetVulkanPipeline().GetVkPipelineCache();
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
}

void MakeAssets(VulkanContext &context) {
  vertex_buffer_collection = new VertexBufferCollection();
  std::vector<float> triangle_vertices = {0.0f, -0.05f, 0.0f,   0.0f,  1.0f, 0.05f, 0.05f, 0.0f,
                                          0.0f, 1.0f,   -0.05f, 0.05f, 0.0f, 0.0f,  1.0f};
  vertex_buffer_collection->Add(MeshType::TRIANGLE, triangle_vertices);

  std::vector<float> square_vertices = {{-0.05f, 0.05f,  1.0f, 0.0f, 0.0f, -0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
                                         0.05f,  -0.05f, 1.0f, 0.0f, 0.0f, 0.05f,  -0.05f, 1.0f, 0.0f, 0.0f,
                                         0.05f,  0.05f,  1.0f, 0.0f, 0.0f, -0.05f, 0.05f,  1.0f, 0.0f, 0.0f}};
  vertex_buffer_collection->Add(MeshType::SQUARE, square_vertices);

  std::vector<float> star_vertices = {
      {-0.05f, -0.025f, 0.0f, 0.0f, 1.0f, -0.02f, -0.025f, 0.0f, 0.0f, 1.0f, -0.03f, 0.0f,    0.0f, 0.0f, 1.0f,
       -0.02f, -0.025f, 0.0f, 0.0f, 1.0f, 0.0f,   -0.05f,  0.0f, 0.0f, 1.0f, 0.02f,  -0.025f, 0.0f, 0.0f, 1.0f,
       -0.03f, 0.0f,    0.0f, 0.0f, 1.0f, -0.02f, -0.025f, 0.0f, 0.0f, 1.0f, 0.02f,  -0.025f, 0.0f, 0.0f, 1.0f,
       0.02f,  -0.025f, 0.0f, 0.0f, 1.0f, 0.05f,  -0.025f, 0.0f, 0.0f, 1.0f, 0.03f,  0.0f,    0.0f, 0.0f, 1.0f,
       -0.03f, 0.0f,    0.0f, 0.0f, 1.0f, 0.02f,  -0.025f, 0.0f, 0.0f, 1.0f, 0.03f,  0.0f,    0.0f, 0.0f, 1.0f,
       0.03f,  0.0f,    0.0f, 0.0f, 1.0f, 0.04f,  0.05f,   0.0f, 0.0f, 1.0f, 0.0f,   0.01f,   0.0f, 0.0f, 1.0f,
       -0.03f, 0.0f,    0.0f, 0.0f, 1.0f, 0.03f,  0.0f,    0.0f, 0.0f, 1.0f, 0.0f,   0.01f,   0.0f, 0.0f, 1.0f,
       -0.03f, 0.0f,    0.0f, 0.0f, 1.0f, 0.0f,   0.01f,   0.0f, 0.0f, 1.0f, -0.04f, 0.05f,   0.0f, 0.0f, 1.0f}};
  vertex_buffer_collection->Add(MeshType::STAR, star_vertices);

  vertex_buffer_collection->Finalize(context.GetVulkanLogicalDevice(), context.GetVulkanPhysicalDevice(),
                                     context.GetVulkanDevice().GetVkGraphicsQueue(),
                                     context.GetVulkanCommandPool().GetVkMainCommandBuffer());
}

void PrepareScene(vk::CommandBuffer command_buffer) {
  vk::Buffer vertex_buffers[] = {vertex_buffer_collection->vertex_buffer_.buffer};
  vk::DeviceSize offsets[] = {0};
  command_buffer.bindVertexBuffers(0, 1, vertex_buffers, offsets);
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
  clear_value.color.float32[3] = 1.0f;
  render_pass_info.clearValueCount = 1;
  render_pass_info.pClearValues = &clear_value;

  command_buffer.beginRenderPass(&render_pass_info, vk::SubpassContents::eInline);
  vk::Pipeline pipeline = context.GetVulkanPipeline().GetVkPipeline();
  command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

  // Gets the vertex buffer data from TriangleMesh and pushes it as uniform data in anticipation for the vertex shader to use.
  PrepareScene(command_buffer);

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

  // we are basically pushing a constant to the vertex shader and rendering the triangle at multiple positions.

  // ------ Draw triangles ------
  int first_vertex = vertex_buffer_collection->offsets_.find(MeshType::TRIANGLE)->second;
  int vertex_count = vertex_buffer_collection->sizes_.find(MeshType::TRIANGLE)->second;
  std::vector<glm::vec3> triangle_positions = currentApp->GetScene().triangle_positions_;

  for (auto position : triangle_positions) {
    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), position);
    command_buffer.pushConstants(pipeline_layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4),
                                 &model_matrix);
    command_buffer.draw(vertex_count, 1, first_vertex, 0);// This draws a triangle - hard coded for now
  }

  // ------ Draw squares ------
  first_vertex = vertex_buffer_collection->offsets_.find(MeshType::SQUARE)->second;
  vertex_count = vertex_buffer_collection->sizes_.find(MeshType::SQUARE)->second;
  std::vector<glm::vec3> square_positions = currentApp->GetScene().square_positions_;
  for (auto position : square_positions) {
    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), position);
    command_buffer.pushConstants(pipeline_layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4),
                                 &model_matrix);
    command_buffer.draw(vertex_count, 1, first_vertex, 0);// This draws a triangle - hard coded for now
  }

  // ------ Draw stars -------
  first_vertex = vertex_buffer_collection->offsets_.find(MeshType::STAR)->second;
  vertex_count = vertex_buffer_collection->sizes_.find(MeshType::STAR)->second;
  std::vector<glm::vec3> star_positions = currentApp->GetScene().star_positions_;
  for (auto position : star_positions) {
    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), position);
    command_buffer.pushConstants(pipeline_layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4),
                                 &model_matrix);
    command_buffer.draw(vertex_count, 1, first_vertex, 0);// This draws a triangle - hard coded for now
  }
}

/**
 * @brief Sets up the necessary Vulkan rendering objects and synchronization objects; gets frame index, resets fences/semaphores, resets command buffer
 *
 * @param context The Vulkan context containing necessary objects for rendering.
 */
static void SetupRender(VulkanContext &context) {
  std::vector<vk::Fence> in_flight_fences = context.GetVulkanSync().GetInFlightFences();
  vk::Device device = context.GetVulkanLogicalDevice();
  vk::SwapchainKHR swap_chain = context.GetVulkanSwapChain().GetVkSwapchain();
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
  VK_CHECK(device.resetFences(1, &in_flight_fences[context.current_frame_index_]), "Failed to reset fences");

  if (res == vk::Result::eErrorOutOfDateKHR || res == vk::Result::eSuboptimalKHR) {
    swapChainRebuild = true;
    return;
  }

  // get the frame's own command buffer
  std::vector<vk::CommandBuffer> frame_command_buffers = context.GetVulkanCommandPool().GetVkFrameCommandBuffers();
  vk::CommandBuffer command_buffer = frame_command_buffers[context.current_frame_index_];
  command_buffer.reset();
}

static void FramePresent(VulkanContext &context) {
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

void SubmitCommandBuffer(VulkanContext &context) {
  std::vector<vk::Fence> in_flight_fences = context.GetVulkanSync().GetInFlightFences();
  vk::Device device = context.GetVulkanLogicalDevice();
  vk::Queue graphics_queue = context.GetVulkanDevice().GetVkGraphicsQueue();
  std::vector<vk::Semaphore> image_available_semaphores = context.GetVulkanSync().GetImageAvailableSemaphores();
  std::vector<vk::Semaphore> render_complete_semaphores = context.GetVulkanSync().GetRenderFinishedSemaphores();

  // current frame command buffer
  vk::CommandBuffer command_buffer =
      context.GetVulkanCommandPool().GetVkFrameCommandBuffers()[context.current_frame_index_];
  command_buffer.endRenderPass();
  command_buffer.end();

  // submit the command buffer
  auto wait_stage = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
  vk::SubmitInfo submit_info = {};
  submit_info.sType = vk::StructureType::eSubmitInfo;
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = &image_available_semaphores[context.semaphore_index_];
  submit_info.pWaitDstStageMask = &wait_stage;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers =
      &context.GetVulkanCommandPool().GetVkFrameCommandBuffers()[context.current_frame_index_];
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &render_complete_semaphores[context.semaphore_index_];

  // fence is provided here so that once we submit the command buffer, we can
  // safely reset the fence
  VK_CHECK(graphics_queue.submit(1, &submit_info, in_flight_fences[context.current_frame_index_]),
           "Failed to submit to queue");
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

#if _DEBUG
  ImGuiInitialize(context, glfw_window);
#endif

  int width, height;
  ImGuiIO &io = ImGui::GetIO();

  // create our giant vertex buffer from "assets"
  MakeAssets(context);

  app->OnStart();
  // ----------------------------- MAIN LOOP ----------------------------- //
  while (!glfwWindowShouldClose(glfw_window)) {
    glfwPollEvents();
    app->OnUpdate();

    glfwGetFramebufferSize(glfw_window, &width, &height);

    if (swapChainRebuild) {
      if (width > 0 && height > 0) {
        GINFO("Rebuilding swapchain... width: {}, height: {}", width, height);
        context.GetVulkanLogicalDevice().waitIdle();
        context.GetVulkanRenderPass().Rebuild();
        context.GetVulkanSwapChain().RebuildSwapChain(width, height);
        context.GetVulkanPipeline().Rebuild();
        context.GetVulkanCommandPool().RebuildCommandBuffers();
        context.GetVulkanSync().Rebuild();
        context.current_frame_index_ = 0;
      }
      swapChainRebuild = false;
    }

    // is window minimized?
    if (width <= 0 || height <= 0) { continue; }

    SetupRender(context);
    RecordDrawCommands(context.GetVulkanCommandPool().GetVkFrameCommandBuffers()[context.current_frame_index_],
                       context.current_frame_index_);

#if _DEBUG
    // ------------------ Render ImGui Frame ------------------ //
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
      //      bool show_demo = true;
      //      ImGui::ShowDemoWindow(&show_demo);

      ImGui::Begin("FPS");
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
      ImGui::End();
    }

    // Rendering
    ImGui::Render();

    // needed for multi-port support
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      // FIXME: moving imgui windows outside glfw window causes validation errors related to render pass (vkCmdDrawIndexed())
    }

    ImDrawData *draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(
        draw_data, context.GetVulkanCommandPool().GetVkFrameCommandBuffers()[context.current_frame_index_]);
#endif

    SubmitCommandBuffer(context);
    FramePresent(context);
  }

  context.GetVulkanLogicalDevice().waitIdle();

#if _DEBUG
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
#endif

  context.Destroy();

  glfwDestroyWindow(glfw_window);
  glfwTerminate();
  app->OnShutdown();
}

}// namespace glaceon
