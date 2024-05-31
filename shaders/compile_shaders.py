import os
import subprocess
import sys


def compile_shader(input_file):
    file_name, file_extension = os.path.splitext(input_file)
    output_file = f"{file_extension[1:]}.spv"

    try:
        subprocess.run(["glslc", input_file, "-o", output_file], check=True)
        print(f"Compilation successful: {input_file} -> {output_file}")
    except subprocess.CalledProcessError as e:
        print(f"Error compiling {input_file}: {e}")


def main(dir_path=None):
    if dir_path:
        shader_directory = dir_path
    else:
        shader_directory = "."

    if not os.path.isdir(shader_directory):
        print(f"Error: {shader_directory} is not a valid directory")
        return

    for filename in os.listdir(shader_directory):
        if filename.endswith(".frag") or filename.endswith(".vert"):
            input_path = os.path.join(shader_directory, filename)
            compile_shader(input_path)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        directory_path = sys.argv[1]
    else:
        directory_path = None
    main(directory_path)
