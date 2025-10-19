#define _HAS_STD_BYTE 0
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "GameObj.h"
#include "Scene.h"
#include "light_objects.h"
#include "GUI.h"


using std::cout, std::endl;

char keyOnce[GLFW_KEY_LAST + 1];
#define glfwGetKeyOnce(WINDOW, KEY)				\
	(glfwGetKey(WINDOW, KEY) ?				\
	 (keyOnce[KEY] ? false : (keyOnce[KEY] = true)) :	\
	 (keyOnce[KEY] = false))

int selectedGameObj = -1;


void setFocus(GLFWwindow* window, Camera scenecam, bool& focus)
{
	if (glfwGetKeyOnce(window, GLFW_KEY_G) == GLFW_PRESS) {
		focus = !focus;
		if (focus==true)
		{
			cout << "focus on"<<endl;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPos(window, scenecam.xposMouse, scenecam.yposMouse);
		}
		else {
			cout << "focus off"<<endl;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			int width, height;
			glfwGetWindowSize(window,&width, &height);
			glfwSetCursorPos(window, width/2, height/2);
			//glfwSetCursorPos(window, 1920 / 2, 1080 / 2);
		}
		cout << focus;
		//return focus;
	}
}

int main() {

	int height = 1080;
	int width = 1920;

	// Basic GLFW window creation
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Renderer", NULL, NULL);

	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, width, height);
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
	GameObject* rock = new GameObject("Models/basecharacter/funnyrock.obj", false, "rock");
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
	diffuseShader.Activate();
	diffuseShader.Setvec3("ambience", ambience);
	//Model test_scene("Assets/scene.obj");
	GameObject* test_scene = new GameObject("Assets/scene.obj", false, "scenetest");
	scene->addGameObject(test_scene, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	bool focus = true;
	while (glfwWindowShouldClose(window) == false) {
		float currentTime = glfwGetTime();
		float dtime = currentTime - time;
		dtime *= 100;
		time = currentTime;
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { break; }   //setting up the close window button
		setFocus(window, scenecam, focus);

		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		emissiveShader.Activate();
		emissiveShader.Setmat4("cameraMatrix", scenecam.GetTransformMatrix());
		//emissiveShader.Setmat4("model", model);

		diffuseShader.Activate();
		diffuseShader.Setmat4("cameraMatrix", scenecam.GetTransformMatrix());
		diffuseShader.Setvec3("cameraPos", scenecam.Position);
		diffuseShader.Setmat4("model",model);
		diffuseShader.Set1f("tmaterial.shine", 64.0f);
		scene->gameObjects[1]->yaxisanglem += dtime;
		//test_scene.Draw(diffuseShader);
		

		scene->render(diffuseShader, emissiveShader);
		scenecam.GetKeyInputs(window, 0.05f, focus, dtime); //Camera movement
		if (!focus)
		{
			selectedGameObj = Gui(selectedGameObj, scene->gameObjects, scene->points, scene->suns, scene->cones);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;

}