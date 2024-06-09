import os
import subprocess
import sys


def compile_shader(input_file_path: str) -> None:
    """
    Compiles a shader file using glslc.

    Parameters:
    input_file_path (str): The path to the shader file to be compiled.

    Returns:
    None
    """
    output_file_path = f"{input_file_path.rsplit('.', 1)[0]}.spv"

    try:
        subprocess.run(
            ["glslc", input_file_path, "-o", output_file_path],
            check=True,
            capture_output=True,
        )
        print(f"Compilation successful: {input_file_path} -> {output_file_path}")
    except subprocess.CalledProcessError as e:
        print(f"Error compiling {input_file_path}: {e.stderr.decode()}")


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
    if not os.path.isdir(directory_path):
        raise ValueError(f"{directory_path} is not a valid directory")

    for filename in os.listdir(directory_path):
        if filename.endswith((".frag", ".vert")):
            shader_path = os.path.join(directory_path, filename)
            compile_shader(shader_path)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        dir_path = sys.argv[1]
        compile_shaders_in_directory(dir_path)
    else:
        print("Usage: python compile_shaders.py <directory_path>")
