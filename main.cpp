#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "GameObj.h"
#include "Scene.h"
#include "light_objects.h"

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

	//Model thing("Assets/spode-christmas-mug/source/mug.obj");
	Scene* scene = new Scene;

	GameObject* bag = new GameObject("Models/bag_model/backpack.obj", true, "bag");
	GameObject* rock = new GameObject("Models/basecharacter/funnyrock.obj", true, "rock");
	GameObject* skull = new GameObject("Models/basecharacter/brideskull.obj", false, "skull");

	scene->addGameObject(bag, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f);
	scene->addGameObject(rock, glm::vec3(5.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f);
	scene->addGameObject(skull, glm::vec3(0.0f, 5.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f);

	//making the lights
	PointLight point(glm::vec3(50.0f, 1.0f, 5.0f), glm::vec3(1.0f), glm::vec3(1.0f), "p1");
	vector<PointLight*> pointLights = {};
	pointLights.push_back(&point);

	SunLight sun(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.8f), glm::vec3(1.0f, 1.0f, 0.8f), "s1");
	vector<SunLight*> sunLights = {};
	sunLights.push_back(&sun);

	ConeLight cone(glm::vec3(0.0f, -10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 1.0f), 20.0f, glm::vec3(1.0f, 1.0f, 0.8f), glm::vec3(1.0f, 1.0f, 0.8f), "c1");
	vector<ConeLight*> coneLights = {};
	coneLights.push_back(&cone);

	scene->points = pointLights;
	scene->suns = sunLights;
	scene->cones = coneLights;

	float time = glfwGetTime();
	
	glm::mat4 model(1.0f);

	while (glfwWindowShouldClose(window) == false) {
		float currentTime = glfwGetTime();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { break; }   //setting up the close window button

		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		diffuseShader.Activate();
		diffuseShader.Setmat4("cameraMatrix", scenecam.GetTransformMatrix());
		diffuseShader.Setvec3("cameraPos", scenecam.Position);
		diffuseShader.Setvec3("ambience", ambience);
		diffuseShader.Setmat4("model",model);

		PassPointsToShader(diffuseShader, scene -> points);
		PassSunsToShader(diffuseShader,scene -> suns);
		PassConesToShader(diffuseShader, scene -> cones);

		//sun.Direction = glm::vec3(0.0f, sin(currentTime), 1.0f);
		float dtime = currentTime - time;

		scenecam.GetKeyInputs(window, 0.01f, true, dtime); //Camera movement
		scene->render(diffuseShader, emissiveShader);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;

}