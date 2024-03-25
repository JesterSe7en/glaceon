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
#define VK_CHECK(f, msg)                                                                                               \
  do {                                                                                                                 \
    vk::Result r = (f);                                                                                              \
    if (static_cast<int>(r) < 0) {                                                                                                     \
      GERROR("Vulkan error: {}", msg);                                                                                 \
      GERROR("Vulkan Validation error: {}", vk::to_string(f));                                                         \
      exit(EXIT_FAILURE);                                                                                              \
    }                                                                                                                  \
  } while (0)

#ifdef _DEBUG
#define VK_ASSERT(expr, msg)                                                                                           \
  do {                                                                                                                 \
    if (!(expr)) {                                                                                                     \
      GERROR("Assertion failed: {}", msg);                                                                             \
      exit(EXIT_FAILURE);                                                                                              \
    }                                                                                                                  \
  } while (0)
#else
#define VK_ASSERT(expr, msg)
#endif

#include "TriangleMesh.h"
#include "SquareMesh.h"
#include "StarMesh.h"


