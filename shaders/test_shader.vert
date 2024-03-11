#version 450

in vec3 position;

void main() {
    // This simply takes the input vector3 position and sets it to
    // the built-in variable gl_Position.
    gl_Position = vec4(position, 1.0);
}