#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANBASE_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANBASE_H_

namespace glaceon {
template<typename T>
void CheckVkResult(T result, const char* msg) {
  if (static_cast<int>(result) < 0) {
    GERROR("Vulkan error: {}", msg);
    GERROR("Vulkan Validation error: {}", vk::to_string(static_cast<vk::Result>(result)));
    exit(EXIT_FAILURE);
  }
}

// res < 0 as some vulkan functions return just a status and not an error
#define VK_CHECK(f, msg) CheckVkResult((f), (msg))

#ifdef _DEBUG
#define VK_ASSERT(expr, msg)               \
  do {                                     \
    if (!(expr)) {                         \
      GERROR("Assertion failed: {}", msg); \
      exit(EXIT_FAILURE);                  \
    }                                      \
  } while (0)
#else
#define VK_ASSERT(expr, msg)
#endif

}// namespace glaceon

#endif//GLACEON_GLACEON_VULKANRENDERER_VULKANBASE_H_
