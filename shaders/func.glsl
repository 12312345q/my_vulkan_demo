vec3 up(vec3 direction){
    const vec3 globalUp = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(globalUp, direction));
    vec3 upVector = normalize(cross(direction, right));
    return upVector;
}

mat4 getViewDirection(vec3 position,vec3 direction){
        mat4 view;
        const vec3 w=normalize(direction) ;
        const vec3 u=normalize(cross(w, up(direction)));
        const vec3 v=cross(w, u) ;

        view = mat4(1.f);
        view[0][0] = u.x;
        view[1][0] = u.y;
        view[2][0] = u.z;
        view[0][1] = v.x;
        view[1][1] = v.y;
        view[2][1] = v.z;
        view[0][2] = w.x;
        view[1][2] = w.y;
        view[2][2] = w.z;
        view[3][0] = -dot(u, position);
        view[3][1] = -dot(v, position);
        view[3][2] = -dot(w, position);
        return view;
}
mat4 setOrthographicProjection(
        float left, float right, float top, float bottom, float near, float far){
        mat4 projectionMatrix;
        projectionMatrix = mat4(1.0f);
        projectionMatrix[0][0] = 2.f / (right - left);
        projectionMatrix[1][1] = 2.f / (top - bottom);
        projectionMatrix[2][2] = 1.f / (far - near);
        projectionMatrix[3][0] = -(right + left) / (right - left);
        projectionMatrix[3][1] = -(bottom + top) / (top - bottom);
        projectionMatrix[3][2] = -near / (far - near);
        return projectionMatrix;
}
mat4 setPerspectiveProjection(float fovy, float aspect, float near, float far){
    const float tanHalfFovy = tan(fovy / 2.f);
    mat4 projectionMatrix = mat4(0.0f);
    projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
    projectionMatrix[1][1] = 1.f / (tanHalfFovy);
    projectionMatrix[2][2] = far / (far - near);
    projectionMatrix[2][3] = 1.f;
    projectionMatrix[3][2] = -(far * near) / (far - near);

    return projectionMatrix;
}

vec2 getScreenPos(vec4 position){
    vec3 ndcPos = position.xyz / position.w;
    return vec2(0.5) * (ndcPos.xy + vec2(1.0));
}

float pseudoRandom(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 778.233))) * 43758.5453);
}

vec2 randomIntInRange(vec2 minVal, vec2 maxVal, vec2 co) {
    vec2 pseudoRand = vec2(pseudoRandom(co), pseudoRandom(co * 2.0)); 
    vec2 range = vec2(maxVal - minVal + 1);
    vec2 result = minVal + ivec2(pseudoRand * range);
    return result;
}

vec3 randomVertical(vec3 v,float time){
    float seed=pseudoRandom(vec2(sin(time),cos(time)));
    vec3 randOther=vec3(sin(0.3*seed),cos(0.5*seed),sin(0.7*seed));
    return normalize(cross(v,randOther));
}

vec2 hash22(vec2 p) {
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
    return fract(sin(p) * 43758.5453);
}

vec2 poissonDiskSampleOnce(vec2 center, float r, float rand) {
    const int gridSize = 16;
    
    vec2 result = vec2(0.0);
    
    int grid[gridSize * gridSize];
    for (int i = 0; i < gridSize * gridSize; ++i) {
        grid[i] = -1;
    }
    
    int activeList[gridSize * gridSize];
    int activeCount = 0;
    
    vec2 firstPoint = center + (hash22(center) - 0.5) * 2.0;
    int firstGridX = int(firstPoint.x * gridSize);
    int firstGridY = int(firstPoint.y * gridSize);
    
    activeList[activeCount++] = firstGridY * gridSize + firstGridX;
    grid[firstGridY * gridSize + firstGridX] = activeCount - 1;
    
    float pointRadius = r;
    
    while (activeCount > 0) {
        int randomIndex = int(rand * float(activeCount)) % activeCount;
        int activeIndex = activeList[randomIndex];
        
        int activeGridX = activeIndex % gridSize;
        int activeGridY = activeIndex / gridSize;
        vec2 activePoint = vec2(float(activeGridX) / float(gridSize),
                                float(activeGridY) / float(gridSize));
        
        bool found = false;
        for (int i = 0; i < 30; ++i) {
            vec2 offset = hash22(vec2(float(i), float(i + 1))) - 0.5;
            vec2 candidate = activePoint + offset * pointRadius * 2.0;
            
            int candidateGridX = int(candidate.x * gridSize);
            int candidateGridY = int(candidate.y * gridSize);
            
            if (candidateGridX < 0 || candidateGridX >= gridSize ||
                candidateGridY < 0 || candidateGridY >= gridSize) {
                continue;
            }
            
            bool close = false;
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    if (candidateGridX + dx >= 0 && candidateGridX + dx < gridSize &&
                        candidateGridY + dy >= 0 && candidateGridY + dy < gridSize) {
                        int neighborIndex = (candidateGridY + dy) * gridSize + candidateGridX + dx;
                        if (grid[neighborIndex] != -1) {
                            float distance = length(candidate - activePoint);
                            if (distance < pointRadius) {
                                close = true;
                                break;
                            }
                        }
                    }
                }
                if (close) {
                    break;
                }
            }
            
            if (!close) {
                found = true;
                int newIndex = candidateGridY * gridSize + candidateGridX;
                activeList[activeCount++] = newIndex;
                grid[newIndex] = activeCount - 1;
                result = candidate;
                break;
            }
        }
        
        if (!found) {
            activeList[randomIndex] = activeList[--activeCount];
        }
    }
    
    return result;
}
