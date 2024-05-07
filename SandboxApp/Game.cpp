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
  const std::string fPath = R"(..\..\models\DefaultGreenCube.glb)";
  const glaceon::Assimp_ModelData content = glaceon::AssimpImporter::ImportObjectModel(fPath);
  PushContent(content);
}
void SandBoxApplication::OnUpdate() {}
void SandBoxApplication::OnShutdown() {}
