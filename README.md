# Glaceon

Glaceon is a 3D rendering project developed with the primary aim of deepening understanding of the fundamental components and inner workings of game engines.
This project utilizes a range of libraries and tools to provide a robust environment for exploring graphics programming and game development concepts.

## Features

- **3D Rendering**: Utilizes Vulkan for efficient and high-performance graphics rendering.
- **Model Loading**: Integrates Assimp for importing and exporting 3D model formats.
- **Math Libraries**: Uses GLM for mathematics needed in graphics programming.
- **Logging**: Employs spdlog for fast and easy logging.
- **Memory Management**: Incorporates Vulkan Memory Allocator for sophisticated memory handling.
- **UI**: Uses ImGui for immediate mode graphical user interfaces.
- **Image Loading**: Utilizes stb_image for image loading.

## Getting Started

### Prerequisites

- [CMake](https://cmake.org/) 3.22.1 or higher
- [Vulkan SDK](https://vulkan.lunarg.com/) 1.3.283.0 or higher
- [vcpkg](https://github.com/microsoft/vcpkg) for managing dependencies

### Building the Project

1. Clone the repository:
    ```sh
    git clone https://github.com/JesterSe7en/glaceon.git
    cd glaceon
    ```

2. Bootstrap vcpkg:
   ```sh
    ./extern/vcpkg/bootstrap-vcpkg.sh
    ```

3. Install dependencies using vcpkg:
    ```sh
    ./extern/vcpkg install
    ```

4. Configure and build the project with CMake:
    ```sh
    mkdir build
    cd build
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
    cmake --build .
    ```

## Libraries Used

- **[Assimp](https://github.com/assimp/assimp)**: Open Asset Import Library
- **[spdlog](https://github.com/gabime/spdlog)**: Fast C++ logging library
- **[Vulkan Memory Allocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)**: Easy to integrate Vulkan memory allocation library
- **[Vulkan](https://www.khronos.org/vulkan/)**: Low-overhead, cross-platform 3D graphics and computing API
- **[GLM](https://github.com/g-truc/glm)**: Header-only C++ mathematics library for graphics software
- **[ImGui](https://github.com/ocornut/imgui)**: Bloat-free graphical user interface library for C++
- **[stb](https://github.com/nothings/stb)**: Single-file public domain libraries for C/C++

## Contributing

Contributions are welcome! Please fork the repository and create a pull request with your changes. I am using Google's C++ coding style so please make sure to follow that coding style.
## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgements

Special thanks to the developers of the libraries and tools used in this project.