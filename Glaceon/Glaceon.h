#pragma once

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

namespace Glaceon {

class GLACEON_API Application {
  Application() = default;
  virtual void onStart() = 0;
  virtual void onUpdate() = 0;
  virtual void onShutdown() = 0;
};

void GLACEON_API runGame();