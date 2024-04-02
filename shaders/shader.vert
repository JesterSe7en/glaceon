#version 450

// Vulkan NDC : x: -1(left), 1(right)
//              y: -1(top), 1(bottom)

//vec2 positions[3] = vec2[](
//    vec2(0.0, -0.05),
//    vec2(0.05, 0.05),
//    vec2(-0.05, 0.05)
//);
//
//vec3 colors[3] = vec3[](
//    vec3(1.0, 0.0, 0.0),
//    vec3(0.0, 1.0, 0.0),
//    vec3(0.0, 0.0, 1.0)
//);

layout (set = 0, binding = 0) uniform UBO {
    mat4 view;
    mat4 proj;
    mat4 view_proj;
} cameraData;

// std140 = defines the memory layout of the bufffer data; ensures the buffer is laid out that is compatible ofr OpenGL standard layout rules
layout (std140, set = 0, binding = 1) readonly buffer storageBuffer {
    mat4 model[];
} ObjectData;

// since we have attribute description now set in the pipeline (vec2(x,y) - position, vec3(rgb) - color layout)
// see VulkanPipeline, "GetPosColorBindingDescription()"
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec2 vertex_tex_coord;


// https://github.com/KhronosGroup/GLSL/blob/main/extensions/khr/GL_KHR_vulkan_glsl.txt
// Look at push constant entry

//layout (push_constant) uniform constants {
//mat4 model;
//} ObjectData;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 fragTextCoord;

void main() {
    //    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    //    gl_Position = ObjectData.model * vec4(positions[gl_VertexIndex], 0.0, 1.0);
    //    fragColor = colors[gl_VertexIndex];

    // instead of using the hardcoded values, we now use passed in data from the graphics pipeline.
    gl_Position = cameraData.view_proj * ObjectData.model[gl_InstanceIndex] * vec4(vertex_position, 0.0, 1.0);
    fragColor = vertex_color;
    fragTextCoord = vertex_tex_coord;
}