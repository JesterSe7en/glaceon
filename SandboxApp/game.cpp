#include "game.h"
#include <cstdio>

int main() {
  SandBoxApplication app;
  Glaceon::runGame(&app);
}

void SandBoxApplication::onStart() {
  fprintf(stdout, "onStart - says hi!\n");

}

void SandBoxApplication::onUpdate() {
  fprintf(stdout, "onUpdate - says hi!\n");

}

void SandBoxApplication::onShutdown() {
  fprintf(stdout, "onShutdown - says hi!\n");
}

SandBoxApplication::SandBoxApplication() {
  myprivatevariable = 100;
  fprintf(stdout, "My private variable is %d\n", myprivatevariable);
}
