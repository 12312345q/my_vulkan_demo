#pragma once
bool framebufferResized = false;
bool canMove = true;
extern std::optional<float>mousex;
extern std::optional<float>mousey;
class Window {
public:
	GLFWwindow* window;
	
	Window() {
		initWindow();
	}

	~Window() {
		glfwDestroyWindow(window);

		glfwTerminate();
	}

	void initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);	
		glfwSetKeyCallback(window, key_callback);
	}

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		framebufferResized = true;
	}

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			canMove = !canMove;
			mousex = std::nullopt;
			mousey = std::nullopt;
			mouseVisible(window);
		}
		
	}
	static void mouseVisible(GLFWwindow* window) {
		if (canMove) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
	

};