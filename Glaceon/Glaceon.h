#pragma once

#ifdef GLACEON_EXPORTS
#define GLACEON_API __declspec(dllexport)
#else
#define GLACEON_API __declspec(dllimport)
#endif

class GLACEON_API Application {
  Application() = default;
  virtual void onStart() = 0;
  virtual void onUpdate() = 0;
  virtual void onShutdown() = 0;
};

void GLACEON_API runGame();