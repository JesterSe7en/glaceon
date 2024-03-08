import os
import subprocess

def compile_shader(input_file):
    file_name, file_extension = os.path.splitext(input_file)
    output_file = f"{file_extension[1:]}.spv"

    try:
        subprocess.run(["glslc", input_file, "-o", output_file], check=True)
        print(f"Compilation successful: {input_file} -> {output_file}")
    except subprocess.CalledProcessError as e:
        print(f"Error compiling {input_file}: {e}")

def main():
    shader_directory = "."

    for filename in os.listdir(shader_directory):
        if filename.endswith(".frag") or filename.endswith(".vert"):
            input_path = os.path.join(shader_directory, filename)
            compile_shader(input_path)

if __name__ == "__main__":
    main()
