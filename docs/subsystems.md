```cpp
int main(int argc, const char* argv[]) {
    // Start up engine systems in the correct order.
    gMemoryManager.startUp();
    gFileSystemManager.startUp();
    gVideoManager.startUp();
    gTextureManager.startUp();
    gRenderManager.startUp();
    gAnimationManager.startUp();
    gPhysicsManager.startUp();
    // ...
    // Run the game.
    gSimulationManager.run();
    // Shut everything down, in reverse order.
    gPhysicsManager.shutDown();
    gAnimationManager.shutDown();
    gRenderManager.shutDown();
    gTextureManager.shutDown();
    gVideoManager.shutDown();
    gFileSystemManager.shutDown();
    gMemoryManager.shutDown();
    return 0;
}
