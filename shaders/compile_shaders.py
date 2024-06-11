import os
import subprocess
import sys
import concurrent.futures


def compile_shader(input_file_path: str) -> None:
    """
    Compiles a shader file using glslc.

    Parameters:
    input_file_path (str): The path to the shader file to be compiled.

    Returns:
    None
    """
    output_file_path = f"{os.path.splitext(input_file_path)[1].lstrip('.')}.spv"

    result = subprocess.run(
        ["glslc", input_file_path, "-o", output_file_path],
        capture_output=True,
        text=True,  # decodes error output
        timeout=30,  # timeout in seconds
    )

    if result.returncode == 0:
        print(f"Compilation successful: {input_file_path} -> {output_file_path}")
    else:
        print(f"Error compiling {input_file_path}: {result.stderr}")


def compile_shaders_in_directory(directory_path: str) -> None:
    """
    Compiles all shader files in the specified directory.

    Parameters:
    directory_path (str): The path to the directory containing shader files.

    Returns:
    None

    Raises:
    ValueError: If the provided directory path is not valid.
    """

    valid_extensions = [".frag", ".vert"]

    if not os.path.isdir(directory_path):
        raise ValueError(f"{directory_path} is not a valid directory")

    # Compile all .frag and .vert files in the directory
    shader_files = [
        os.path.join(directory_path, f)
        for f in os.listdir(directory_path)
        if os.path.isfile(os.path.join(directory_path, f))
        and os.path.splitext(f)[1] in valid_extensions
    ]

    with concurrent.futures.ThreadPoolExecutor() as executor:
        futures = [
            executor.submit(compile_shader, shader_file) for shader_file in shader_files
        ]
        for future in concurrent.futures.as_completed(futures):
            future.result()

    print("Shader compilation complete")


if __name__ == "__main__":
    # Check if glslc is installed
    process = subprocess.run(["glslc", "--version"], capture_output=True, timeout=10)
    if process.returncode != 0:
        print("Error: glslc not found. Please install it via Vulkan SDK.")
        sys.exit(1)

    if len(sys.argv) > 1:
        dir_path = sys.argv[1]
        compile_shaders_in_directory(dir_path)
    else:
        print("Usage: python compile_shaders.py <directory_path>")
        sys.exit(1)
