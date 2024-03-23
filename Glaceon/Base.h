#ifndef GLACEON_GLACEON_BASE_H_
#define GLACEON_GLACEON_BASE_H_

#ifdef _WIN64
#ifdef GLACEON_EXPORTS
#define GLACEON_API __declspec(dllexport)
#else
#define GLACEON_API __declspec(dllimport)
#endif
#elif __linux__
#ifdef GLACEON_EXPORTS
#define GLACEON_API __attribute__((visibility("default")))
#else
#define GLACEON_API
#endif
#else
#error "Unsupported platform"
#endif

#endif// GLACEON_GLACEON_BASE_H_

// res < 0 as some vulkan functions return just a status and not an error
#define VK_CHECK(res)                                                                                                  \
  {                                                                                                                    \
    if (static_cast<int>(res) < 0) {                                                                                   \
      GERROR("Vulkan error: {}", vk::to_string(res));                                                                  \
      abort();                                                                                                         \
    }                                                                                                                  \
  }

#include "TriangleMesh.h"
