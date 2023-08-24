#version 450
#include"func.glsl"

layout(push_constant) uniform Push{
    mat4 modelMatrix;
    mat4 normalMatrix;
    int num;
}push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 uv;

layout(set = 0, binding = 3) uniform Light{
    vec3 position;
    vec3 direction;
    vec4 color;
    float range;
    float near;
    float far;
    int size;
}light;

layout(location = 0) out vec3 fragPosition;

void main(){


    gl_Position=
    setOrthographicProjection(-light.range,light.range,light.range,-light.range,light.near,light.far)*
    getViewDirection(light.position,light.direction)*
    push.modelMatrix* vec4(inPosition, 1.0);

    fragPosition =(push.modelMatrix* vec4(inPosition, 1.0)).xyz;
}