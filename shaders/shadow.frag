#version 450
layout(set = 0, binding = 3) uniform Light{
    vec3 position;
    vec3 direction;
    vec4 color;
    float range;
    float near;
    float far;
    int size;
}light;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec2 shadowData;
#define PACK_RATIO 1.0
void main(){
    
    float depth = length(light.position-inPosition);
    float depthSquare = pow(depth,2);
    shadowData=vec2(log(depth*PACK_RATIO),log(depthSquare*PACK_RATIO));
}