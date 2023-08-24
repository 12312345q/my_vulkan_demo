#pragma once
std::optional<float>mousex;
std::optional<float>mousey;
class CameraController {
public:
    int left = GLFW_KEY_A;
    int right = GLFW_KEY_D;
    int forward = GLFW_KEY_W;
    int back = GLFW_KEY_S;
    int up = GLFW_KEY_Q;
    int down = GLFW_KEY_E;

    float speed = 0.008f;
    float response = 0.05f;

    glm::vec3 position;
    glm::vec3 direction;
    glm::mat4 proj;
    
    glm::vec3 getRight() {
        return glm::normalize(glm::vec3(direction[2], 0, -direction[0]));
    };
    glm::vec3 getUp() {
        return glm::normalize(glm::cross(direction, getRight()));
    };
    
    CameraController(glm::vec3 pos, glm::vec3 direction) : position{ pos }, direction{ glm::normalize(direction) }{}

    void setPerspectiveProjection(float fovy, float aspect, float near, float far) {
        assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
        const float tanHalfFovy = tan(fovy / 2.f);
        glm::mat4 projectionMatrix = glm::mat4{ 0.0f };
        projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
        projectionMatrix[1][1] = 1.f / (tanHalfFovy);
        projectionMatrix[2][2] = (far+near) / (far - near);
        projectionMatrix[2][3] = 1.f;
        projectionMatrix[3][2] = -(2.0f*far * near) / (far - near);
        proj= projectionMatrix;
    }

    void setOrthographicProjection(
        float left, float right, float top, float bottom, float near, float far) {
        
        glm::mat4 projectionMatrix;
        projectionMatrix = glm::mat4{ 1.0f };
        projectionMatrix[0][0] = 2.f / (right - left);
        projectionMatrix[1][1] = 2.f / (top - bottom);
        projectionMatrix[2][2] = 1.f / (far - near);
        projectionMatrix[3][0] = -(right + left) / (right - left);
        projectionMatrix[3][1] = -(bottom + top) / (top - bottom);
        projectionMatrix[3][2] = -near / (far - near);

        proj = projectionMatrix;
    }

    glm::mat3 rotate_axis(glm::vec3 axis, float angle) {
        float c = glm::cos(angle);
        float s = glm::sin(angle);
        glm::mat3 rotate = {
            {
                c + (1.0 - c) * axis.x * axis.x,
                (1.0 - c) * axis.x * axis.y + s * axis.z,
                (1.0 - c) * axis.x * axis.z - s * axis.y
            },
            {
                (1.0 - c) * axis.x * axis.y - s * axis.z,
                c + (1.0 - c) * axis.y * axis.y,
                (1.0 - c) * axis.y * axis.z + s * axis.x
            },
            {
                (1.0 - c) * axis.x * axis.z + s * axis.y,
                (1.0 - c) * axis.y * axis.z - s * axis.x,
                c + (1.0 - c) * axis.z * axis.z
            }

        };
        return rotate;
    }

    void rotate(float x_, float y_) {
        direction = glm::normalize(rotate_axis(getUp(), x_) * direction);
        direction = glm::normalize(rotate_axis(getRight(), y_) * direction);


    }

    void move(float forward, float right) {
        position += forward * direction + right * getRight();
    }

    void goUp(float up) {
        position.y += up;
    }

    

    void keyboardListener(GLFWwindow* window) {
        if (!canMove) return;
    
        float tMove = 0.f;
        float rMove = 0.f;
        float yMove = 0.f;

        if (glfwGetKey(window, left)!= GLFW_RELEASE) { rMove += speed; }
        if (glfwGetKey(window, right) != GLFW_RELEASE) { rMove -= speed; }
        if (glfwGetKey(window, forward) != GLFW_RELEASE) { tMove += speed; }
        if (glfwGetKey(window, back) != GLFW_RELEASE) { tMove -= speed; }
        if (glfwGetKey(window, up) != GLFW_RELEASE) { yMove += speed; }
        if (glfwGetKey(window, down) != GLFW_RELEASE) { yMove -= speed; }

        move(tMove, rMove);
        goUp(yMove);

    }

    void mouseListener(GLFWwindow* win) {
        if (!canMove)return;
        double xpos, ypos;
        glfwGetCursorPos(win, &xpos, &ypos);
        float x_ = 0.f;
        float y_ = 0.f;
        if (mousex.has_value()) {
            x_ = xpos - mousex.value();
        }
        if (mousex.has_value()) {
            y_ = ypos - mousey.value();
        }
        mousex = xpos;
        mousey = ypos;
        rotate(glm::radians(-x_ * response), glm::radians(y_ * response));

    }
};

