#version 450
#extension GL_EXT_samplerless_texture_functions : enable
#include"func.glsl"
layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 proj;
    vec3 position;
    vec3 direction;
} ubo;
layout(set = 0, binding = 1) uniform sampler samp;
layout(set = 0, binding = 5) uniform texture2D depthMap;
layout(set = 0, binding = 6) uniform texture2D texNoise;
layout(set = 0, binding = 7) uniform texture2D src;
layout(set = 0, binding = 8) uniform texture2D positionMap;
layout(set = 0, binding = 9) uniform texture2D normalMap;

layout(location = 0)in vec2 fragUv;
layout(location = 0) out vec4 outColor;

const vec2 noiseScale = vec2(209/4.0, 215/4.0);
#define PI 3.1415926535897932384626433832795
#define RADIUS 1.5
#define NUMBER_OF_SAMPLING_DIRECTIONS 16
#define STEP 0.002
#define NUMBER_OF_STEPS 4
#define TANGENT_BIAS 0.3
#define BIAS radians(30.0)


vec4 depthToPosition(vec2 uv) {
    float depth = texture(sampler2D(depthMap,samp), uv).x;
 
    vec4 clipSpace = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 viewSpace = inverse(ubo.proj) * clipSpace;

    viewSpace.xyz /= viewSpace.w;
 
    return vec4(vec3(viewSpace), 1.0);
}
 
vec4 depthToNormal(vec2 tc){
    float depth = texture(sampler2D(depthMap,samp), tc).x;
 
    vec4 clipSpace = vec4(tc * 2.0 - 1.0, depth, 1.0);
    vec4 viewSpace = inverse(ubo.proj) * clipSpace;
    viewSpace.xyz /= viewSpace.w;
 
    vec3 pos = viewSpace.xyz;
    vec3 n = normalize(cross(dFdx(pos), dFdy(pos)));
    n *= - 1;
 
    return vec4(n, 1.0);
}

float hbao(vec2 uv){
    float sum = 0.0;
    float occlusion = 0.0;

    vec3 pos = vec3(vec2(uv.x, uv.y), texture(sampler2D(depthMap,samp), vec2(uv.x, uv.y)).r);
 
    vec4 normal = depthToNormal(vec2(uv.x, uv.y));
 
    vec3 NDC_POS = (2.0 * pos) - 1.0; 
    vec4 unprojectPosition = inverse(ubo.proj) * vec4(NDC_POS, 1.0);
    vec3 viewPosition = unprojectPosition.xyz / unprojectPosition.w;
 
    vec3 sampleNoise = texture(sampler2D(texNoise,samp), vec2(uv.x, uv.y) * noiseScale).xyz;
    sampleNoise.xy = sampleNoise.xy * 2.0 - vec2(1.0);
    mat2 rotationMatrix = mat2(sampleNoise.x, -sampleNoise.y, sampleNoise.y, sampleNoise.x);
 
    float samplingDiskDirection = 2 * PI / NUMBER_OF_SAMPLING_DIRECTIONS;
 
 
    for(int i = 0; i < NUMBER_OF_SAMPLING_DIRECTIONS; i++) {
 
        float samplingDirectionAngle = i * samplingDiskDirection;

        vec2 samplingDirection = rotationMatrix * vec2(cos(samplingDirectionAngle), sin(samplingDirectionAngle));

        float tangentAngle = acos(dot(vec3(samplingDirection, 0.0), normal.xyz)) - (0.5 * PI) + TANGENT_BIAS;
        float horizonAngle = tangentAngle; 
 
        vec3 LastDifference = vec3(0);
 
        for(int j = 0; j < NUMBER_OF_STEPS; j++){
 
            vec2 stepForward = float(j+1) * STEP * samplingDirection;

            vec2 stepPosition = vec2(uv.x, uv.y) + stepForward;

            float steppedLocationZ = texture(sampler2D(depthMap,samp), stepPosition.st).r;

            vec3 steppedLocationPosition = vec3(stepPosition, steppedLocationZ);

            vec3 steppedPositionNDC = (2.0 * steppedLocationPosition) - 1.0;
            vec4 SteppedPositionUnProj = inverse(ubo.proj) * vec4(steppedPositionNDC, 1.0);
            vec3 viewSpaceSteppedPosition = SteppedPositionUnProj.xyz / SteppedPositionUnProj.w;
 
 
            vec3 diff = viewSpaceSteppedPosition.xyz - viewPosition;

            if(length(diff) < RADIUS){
 
                LastDifference = diff;
                float FoundElevationAngle = atan(diff.z / length(diff.xy));
                horizonAngle = max(horizonAngle, FoundElevationAngle);
            }
        }
 
            float norm = length(LastDifference) / RADIUS;
            float attenuation = 1 - norm * norm;
 
            occlusion = clamp(attenuation * (sin(horizonAngle+BIAS) - sin(tangentAngle)), 0.0, 1.0);
            sum += 1-occlusion;
    }
 
    return sum /= NUMBER_OF_SAMPLING_DIRECTIONS;

}

float gaussianKernel[5][5] = {
    { 1,  4,  6,  4, 1 },
    { 4, 16, 24, 16, 4 },
    { 6, 24, 36, 24, 6 },
    { 4, 16, 24, 16, 4 },
    { 1,  4,  6,  4, 1 }
};

vec3 bilateralFilter(vec2 uv, float centerDepth) {
    vec2 textureSize = textureSize(depthMap, 0);
    float offsetX = 1.0 / textureSize.x;
    float offsetY = 1.0 / textureSize.y;

    vec3 colorSum = vec3(0);
    float weightSum = 0.0; // 初始化权重和

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            vec2 uvOff = uv + vec2((i - 2) * offsetX, (j - 2) * offsetY);
            float depthGet = texture(sampler2D(depthMap, samp),uvOff).x; // 使用正确的纹理函数
            if (abs(depthGet - centerDepth) <= 0.1) { // 使用 abs 函数计算深度差
                colorSum += texture(sampler2D(src, samp),uvOff).xyz * gaussianKernel[i][j];
                weightSum += gaussianKernel[i][j];
            }
        }
    }
    
    return colorSum / weightSum;
}


int range = 1;
float sigmod = 0.2;
bool edgeDetection(vec2 uv){
    bool isEdge = false;

    vec3 normal=texture(sampler2D(normalMap,samp),uv).xyz;

    vec2 textureSize = textureSize(normalMap, 0);
    float offsetX=1.0/textureSize.x;
    float offsetY=1.0/textureSize.y;

    for(int i=-range;i<range;i++){
        for(int j=-range;j<=range;j++){
            vec3 normalTest=texture(sampler2D(normalMap,samp),uv+vec2(i*offsetX,j*offsetY)).xyz;
            if(length(normal-normalTest)>sigmod){
                isEdge=true;
                break;
            }

        }
    }

    return isEdge;
}


void main(){

//need two pass to hbao and filter,but I make only one
//filteredColor is filter without hbao
//hbaoColor is hbao without filter
//do it another day...


    float ao=hbao(fragUv);
    float dep = texture(sampler2D(depthMap,samp),fragUv).x;

    vec3 filteredColor = bilateralFilter(fragUv, dep);
    vec3 srcColor=texture(sampler2D(src,samp),fragUv).xyz;
    vec3 hbaoColor=srcColor*ao;

    if(edgeDetection(fragUv)){
        outColor = vec4(0,0,0,1.0);
    }else{
        outColor = vec4(hbaoColor,1.0);
    }




}