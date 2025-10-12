#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using std::cout, std::endl;

int main() {

	// Basic GLFW window creation
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 4); // Anti-aliasing
	//glEnable(GL_MULTISAMPLE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "Renderer", NULL, NULL);

	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, 800, 600);
	glEnable(GL_DEPTH_TEST);


	// Clearing background color and rendering loop


	while (glfwWindowShouldClose(window) == false) {
		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
	
}