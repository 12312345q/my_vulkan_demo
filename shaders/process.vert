#version 450
layout(location = 0) in vec3 inUv;
layout(location = 0) out vec2 fragUv;
void main(){
    gl_Position = vec4(inUv, 1.0);
    fragUv=(inUv.xy+vec2(1.0))*0.5;
}