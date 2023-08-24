#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in float inDepth;

layout(location = 0) out vec4 positionBuffer;
layout(location = 1) out vec4 normalBuffer;

void main(){
    positionBuffer = vec4(inPosition,inDepth);
    normalBuffer = vec4(inNormal,1.0);
}