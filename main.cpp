#define _HAS_STD_BYTE 0
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "GameObj.h"
#include "Scene.h"
#include "light_objects.h"
#include "GUI.h"
#include "FB.h"


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
		//cout << focus;
		//return focus;
	}
}


void renderQuad()   //This is for debugging framebuffers.
{

		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO

		unsigned int quadVAO;
		unsigned int quadVBO;
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

int main() {

	int width = 1600; 
	int height = 900;

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

	Shader dqShader("debug_quad.vert", "debug_quad.frag");
	Shader diffuseShader("lighting.vert", "diffuse.frag"); // create shaders
	Shader emissiveShader("emissive.vert", "emissive.frag");
	Shader depthShader("depth.vert", "depth.frag");  // FASTER SHADER TO CALCULATE DEPTH MAPS FOR SHADOWS.
	Camera scenecam(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), 45.0f, (float)(width) / height, 0.1f, 100.0f);  //creating the camera

	//Model thing("Assets/spode-christmas-mug/source/mug.obj");
	//The following code will simply create a scene with some objects and lights.
	Scene* scene = new Scene;

	GameObject* bag = new GameObject("Models/bag_model/backpack.obj", true, "bag");
	GameObject* rock = new GameObject("Models/basecharacter/funnyrock.obj", false, "rock");
	GameObject* skull = new GameObject("Models/basecharacter/brideskull.obj", false, "skull");

	scene->addGameObject(bag, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f);
	scene->addGameObject(rock, glm::vec3(5.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f);
	scene->addGameObject(skull, glm::vec3(0.0f, 5.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f);

	//making the lights
	PointLight point(glm::vec3(0.0f, 5.0f, 5.0f), glm::vec3(1.0f), glm::vec3(1.0f), "p1");
	vector<PointLight*> pointLights = {};
	pointLights.push_back(&point);

	SunLight sun(glm::vec3(0.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 0.8f), glm::vec3(1.0f, 1.0f, 0.8f), "s1");
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

	GLint maxTextures;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextures);
	printf("Max texture units: %d\n", maxTextures);

	//Shadow Map testing
	int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	Framebuffer FB(1024, 1024, "depth");   //FRAME BUFFER FOR DEPTH 
	dqShader.Activate();
	dqShader.Set1i("depthMap", 0);
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

		diffuseShader.Activate();
		diffuseShader.Setmat4("cameraMatrix", scenecam.GetTransformMatrix());
		diffuseShader.Setvec3("cameraPos", scenecam.Position);

		diffuseShader.Set1f("tmaterial.shine", 64.0f);  // Shininess of material. Currently, the game object can't handle this so its hardcoded here. FIX!!!
		scene->gameObjects[1]->yaxisanglem += dtime; // random rotation.


		// shadow pass
		glCullFace(GL_FRONT);
		depthShader.Activate();
		depthShader.Setmat4("lightSpaceMatrix", sun.get_LSmatrix(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f));
		FB.bind();
		scene->depthPass(depthShader);   
		glCullFace(GL_BACK);
		FB.unbind();
		glViewport(0, 0, width, height);   // make sure to reset your viewport.
		//dqShader.Activate();
		//dqShader.Set1f("near_plane", 1.0f);
		//dqShader.Set1f("far_plane", 7.5f);
		//glActiveTexture(GL_TEXTURE0);  // Lets try the 6th texture unit for now.
		//glBindTexture(GL_TEXTURE_2D, depthMap);
		//renderQuad();

		diffuseShader.Activate();

		diffuseShader.Setmat4("LSMatrix", sun.get_LSmatrix(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f));
		diffuseShader.Set1i("shadowMap", 5); 
		FB.bindTexture(5);
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

