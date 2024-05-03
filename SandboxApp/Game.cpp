#include "Game.h"

#include <Assimp/AssimpImporter.h>

#include "Application.h"

int main() {
  SandBoxApplication app;
  glaceon::RunGame(&app);
}
SandBoxApplication::SandBoxApplication() : glaceon::Application(&app_info) {

  // we want to somehow pass a list of vec3 to engine as a push constant
}

void SandBoxApplication::OnStart() {
  // load in object models?
  const std::string fPath = R"(..\..\models\basic_brickCube.obj)";

  glaceon::AssimpImporter::ImportObjectModel(fPath);
}
void SandBoxApplication::OnUpdate() {}
void SandBoxApplication::OnShutdown() {}
