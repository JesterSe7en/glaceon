#include "pch.h"
#include "Glaceon.h"
#include <iostream>
#include <GLFW/glfw3.h>

namespace Glaceon {

void GLACEON_API runGame(Application *app) {
  if (!app) {
    std::cout << "Application is null" << std::endl;
    return;
  }

  if (!glfwInit()) {
    std::cout << "GLFW initialization failed" << std::endl;
    return;
  }



  app->onStart();
  app->onUpdate();

  glfwTerminate();
  app->onShutdown();
}

} // namespace Glaceon
