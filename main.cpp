#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

using std::cout, std::endl;

int main() {

	int height = 800;
	int width = 800;

	// Basic GLFW window creation
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(height, width, "Renderer", NULL, NULL);

	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, height, width);
	glEnable(GL_DEPTH_TEST);

	glfwWindowHint(GLFW_SAMPLES, 4); // Anti-aliasing
	glEnable(GL_MULTISAMPLE);
	// Clearing background color and rendering loop

	glm::vec3 ambience(0.2f, 0.2f, 0.2f);


	Shader diffuseShader("lighting.vert", "diffuse.frag"); // create shaders
	Shader emissiveShader("emissive.vert", "emissive.frag");
	Camera scenecam(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), 45.0f, (float)(width) / height, 0.1f, 100.0f);  //creating the camera

	Model thing("Assets/spode-christmas-mug/source/mug.obj");

	float time = glfwGetTime();
	
	glm::mat4 model(1.0f);

	while (glfwWindowShouldClose(window) == false) {
		float currentTime = glfwGetTime();
		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		diffuseShader.Activate();
		diffuseShader.Setmat4("cameraMatrix", scenecam.GetTransformMatrix());
		diffuseShader.Setvec3("camPos", scenecam.Position);
		diffuseShader.Setvec3("ambience", ambience);
		diffuseShader.Setmat4("model",model);
		thing.Draw(diffuseShader);


		float dtime = currentTime - time;

		scenecam.GetKeyInputs(window, 0.05f, true, dtime); //Camera movement
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;

}