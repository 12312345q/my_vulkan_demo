#pragma once
class Transform {
public:
	glm::vec3 translation;
	glm::vec3 rotation;
    glm::vec3 scale;

    Transform() {
        translation = { 0.f,0.f,0.f };
        rotation = { 0.f,0.f,0.f };
        scale = { 1.f,1.f,1.f };
    }
    Transform(glm::vec3 translation, glm::vec3 rotation) :translation{ translation }, rotation{ rotation }{};
    
    void setTranslation(float x, float y, float z) {
        translation.x = x;
        translation.y = y;
        translation.x = x;
    }

    void setRotation(float x, float y, float z) {
        rotation.x = x;
        rotation.y = y;
        rotation.z = z;
    }

    void setScale(float x, float y, float z) {
        scale.x = x;
        scale.y = y;
        scale.z = z;
    }

	glm::mat4 getModelMatrix() {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        return glm::mat4{
            {
                scale.x * (c1 * c3 + s1 * s2 * s3),
                scale.x * (c2 * s3),
                scale.x * (c1 * s2 * s3 - c3 * s1),
                0.0f,
            },
            {
                 scale.y * (c3 * s1 * s2 - c1 * s3),
                 scale.y * (c2 * c3),
                 scale.y * (c1 * c3 * s2 + s1 * s3),
                 0.0f,
            },
            {
                 scale.z * (c2 * s1),
                 scale.z * (-s2),
                 scale.z * (c1 * c2),
                 0.0f,
            },
            {translation.x, translation.y, translation.z, 1.0f} 
        };
	}

    glm::mat4 getNormalMatrix() {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        const glm::vec3 invScale = 1.0f / scale;

        return glm::mat4{
            {
                invScale.x * (c1 * c3 + s1 * s2 * s3),
                invScale.x * (c2 * s3),
                invScale.x * (c1 * s2 * s3 - c3 * s1),
                0.0f,
            },
            {
                invScale.y * (c3 * s1 * s2 - c1 * s3),
                invScale.y * (c2 * c3),
                invScale.y * (c1 * c3 * s2 + s1 * s3),
                0.0f,
            },
            {
                invScale.z * (c2 * s1),
                invScale.z * (-s2),
                invScale.z * (c1 * c2),
                0.0f,
            },
            { 0.0f , 0.0f , 0.0f , 1.0f }
        };
    }
};