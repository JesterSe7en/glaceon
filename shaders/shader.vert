#version 450

// Vulkan NDC : x: -1(left), 1(right)
//              y: -1(top), 1(bottom)

vec2 positions[3] = vec2[](
    vec2(0.0, -0.05),
    vec2(0.05, 0.05),
    vec2(-0.05, 0.05)
);

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);


// https://github.com/KhronosGroup/GLSL/blob/main/extensions/khr/GL_KHR_vulkan_glsl.txt
// Look at push constant entry

layout (push_constant) uniform constants {
    mat4 model;
} ObjectData;

layout (location = 0) out vec3 fragColor;

void main() {
    //    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    gl_Position = ObjectData.model * vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
}