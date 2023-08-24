#version 450
#extension GL_EXT_samplerless_texture_functions : enable
#extension GL_EXT_nonuniform_qualifier : enable
#include "func.glsl"
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
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec4 fragPosition;
layout(location = 2) in vec2 fragUv;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler samp;
layout(set = 0, binding = 2) uniform texture2D textures[];
layout(set = 0, binding = 3) uniform Light{
    vec3 position;
    vec3 direction;
    vec4 color;
    float range;
    float near;
    float far;
    float size;
    
}light;
layout(set = 0, binding = 4) uniform texture2D shadowData;
layout(set = 0, binding = 5) uniform texture2D depthMap;
layout(set = 0, binding = 6) uniform texture2D texNoise;



float fit01(float distance){
    float cosA=dot(normalize(light.direction),normalize(fragPosition.xyz-light.position));
    distance*=cosA;
    distance-=light.near;
    distance/=(light.far-light.near);
    return distance;
}


#define SAMAPLE_TIMES 32
#define PACK_RATIO 1.0
float pcss(float distance,vec2 uv){
    vec2 textureSize = textureSize(shadowData, 0);
    float offsetX=1.0/textureSize.x;
    float offsetY=1.0/textureSize.y;

    float ratio = distance-1.0;
    float size=light.size*ratio;

    float lightIn=0;


    for(int i=0;i<SAMAPLE_TIMES*ratio;i++){
        vec2 seed=uv+vec2(float(i))*0.1;
        vec2 sampuv=randomIntInRange(vec2(-size,-size),vec2(size,size),seed);
        float depthLog= texture(sampler2D(shadowData,samp),uv+sampuv*vec2(offsetX,offsetY)).x;
        float depth=exp(depthLog)/PACK_RATIO;
        if(depth>=distance-0.02){
            lightIn++;
        }
    }



    return float(lightIn)/SAMAPLE_TIMES;

}
#define PI 3.1415926
float vsm(float distance,vec2 uv){
    float lightBias=0.1;

    float depth=exp(texture(sampler2D(shadowData,samp),uv).x)/PACK_RATIO;
    float depthSquare=exp(texture(sampler2D(shadowData,samp),uv).y)/PACK_RATIO;

    float variance=max(0.001,depthSquare-pow(depth,2));

    float shadowFactor = 0.0;
    float d = distance - depth+0.02;
    if (d > 0.0) {
        shadowFactor = exp(-d * d / (2.0 * variance + lightBias)) / sqrt(2.0 * PI * variance + lightBias);
    }

    return shadowFactor;

}


const float shininess = 8.0;

float Phong(float shadowRatio){
    float ambient=light.color.w*0.08;
    vec3 lightInto=light.position-fragPosition.xyz;
    vec3 cameraInto=ubo.position-fragPosition.xyz;
    vec3 halfV=normalize(lightInto+cameraInto);

    float diffuse=max(dot(normalize(lightInto),normalize(fragNormal)),0);

    float specular = pow(max(dot(normalize(halfV),normalize(fragNormal)),0),shininess);

    float distance=pow(length(lightInto),-2);

    return ambient+shadowRatio*(diffuse+specular)*distance;
}



const int color_num = 4; 

const float colorGrads[color_num]={0.5,0.75,0.815,1};


float Toon(float dotData,float shadowRatio){

    for(int i=0;i<color_num;i++){
        if(dotData<=colorGrads[i]){
            return float(i) * shadowRatio / float(color_num);
        }

    }

    
}

void main(){

    vec4 lightSeePosition=
        setOrthographicProjection(-light.range,light.range,light.range,-light.range,light.near,light.far)*
        getViewDirection(light.position,light.direction)*
        fragPosition;

    vec2 uv=getScreenPos(lightSeePosition); 

    float depthThis=length(fragPosition.xyz-light.position);

    float ratio=pcss(depthThis,uv);

    vec3 lightResult = Phong(ratio)*light.color.xyz*light.color.w;

    float dt=dot(normalize(-light.direction),normalize(fragNormal));

    vec3 ToonResult = (Toon(dt,ratio)+0.2)*light.color.xyz*light.color.w;




    outColor = vec4(ToonResult,1.0)*texture(sampler2D(textures[push.num],samp), fragUv);
}