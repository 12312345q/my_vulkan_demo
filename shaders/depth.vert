#version 450
#include"func.glsl"

layout(push_constant) uniform Push{
    mat4 modelMatrix;
    mat4 normalMatrix;
    int num;
}push;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 proj;
    vec3 position;
    vec3 direction;
} ubo;
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out float depth;

void main(){
    gl_Position =ubo.proj*getViewDirection(ubo.position,ubo.direction)*push.modelMatrix* vec4(inPosition, 1.0);
    position = (push.modelMatrix* vec4(inPosition+0.1*inNormal, 1.0)).xyz;
    normal = inNormal;
    depth = length((getViewDirection(ubo.position,ubo.direction)*push.modelMatrix* vec4(inPosition+0.1*inNormal, 1.0)).xyz);
}