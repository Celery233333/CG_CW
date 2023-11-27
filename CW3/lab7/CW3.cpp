#include <GL/gl3w.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "camera.h"
#include "error.h"
#include "shader.h"
#include "shadow.h"
#include "node.h"
#include "objFile.h"
#include "jeep.h"
#include "terrain.h"
#include "floorLamp.h"
#include "light.h"
#include "curve.h"
#include "skybox.h"
#include "q.h"



#define SH_MAP_WIDTH 4096
#define SH_MAP_HEIGHT 4096
int	g_WindowWidth = 1024;
int	g_WindowHeight = 768;
bool	g_TraceJeep = false;;
Light* g_FloorLamps[2];
glm::vec3 g_ambient(0.4, 0.4, 0.4);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		g_Jeep->switchMove();
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		g_TraceJeep = !g_TraceJeep;
		if (g_TraceJeep) {
			g_camera->setCenter(g_Jeep->getPosition());
		}
	}
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		g_Jeep->turnLight();
	}
	else if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
		g_FloorLamps[0]->turn();
	}
	else if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
		g_FloorLamps[1]->turn();
	}
}

void SizeCallback(GLFWwindow* window, int w, int h)
{
	if (w > 0 && h > 0) {
		g_WindowWidth = w;
		g_WindowHeight = h;
		glViewport(0, 0, w, h);
		g_camera->resetProjectionMatrix(w, h);
	}
}


void renderWithShadow(unsigned int program, ShadowStruct shadow, glm::mat4 projectedLightSpaceMatrix,Node* rootNode)
{
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(g_camera->getViewMatrix()));
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(g_camera->getProjectionMatrix()));
	glUniform3fv(glGetUniformLocation(program, "camPos"), 1, glm::value_ptr(g_camera->getPosition()));
	glUniform3fv(glGetUniformLocation(program, "ambient"), 1, glm::value_ptr(g_ambient));
	glUniformMatrix4fv(glGetUniformLocation(program, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));

	int lightsCount = 0;
	for (Light* light : g_Lights) {
		if (light->isOn()) {
			lightsCount++;
		}
	}
	glUniform1i(glGetUniformLocation(program, "lightsCount"), lightsCount);
	int i = 0;
	for (Light* light : g_Lights) {
		if (!light->isOn()) continue;
		std::string name = "lights[" + std::to_string(i);
		glUniform1i(glGetUniformLocation(program, (name + "].type").c_str()), light->getType());
		glUniform3fv(glGetUniformLocation(program, (name + "].pos").c_str()), 1, glm::value_ptr(light->getLightPosition()));
		glUniform3fv(glGetUniformLocation(program, (name + "].color").c_str()), 1, glm::value_ptr(light->getColor()));
		glUniform1f(glGetUniformLocation(program, (name + "].constAttenuation").c_str()), light->getConstAttenuation());
		glUniform1f(glGetUniformLocation(program, (name + "].linearAttenuation").c_str()), light->getLinearAttenuation());
		glUniform1f(glGetUniformLocation(program, (name + "].quadraticAttenuation").c_str()), light->getQuadraticAttenuation());
		glUniform3fv(glGetUniformLocation(program, (name + "].spotDirection").c_str()), 1, glm::value_ptr(light->getSpotDirection()));
		glUniform1f(glGetUniformLocation(program, (name + "].spotCutoff").c_str()), light->getSpotCutoff());

		i++;
	}

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, shadow.Texture);
	glUniform1i(glGetUniformLocation(program, "shadowMap"), 1);

	glUniform1i(glGetUniformLocation(program, "diffuseMap"), 0);
	glActiveTexture(GL_TEXTURE0);
	rootNode->render(program);
}

void generateDepthMap(unsigned int shadowShaderProgram, ShadowStruct shadow, glm::mat4 projectedLightSpaceMatrix,Node* rootNode)
{
	glViewport(0, 0, SH_MAP_WIDTH, SH_MAP_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadow.FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(shadowShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));
	rootNode->renderDepth(shadowShaderProgram);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool g_MouseDown;//Whether the right mouse button is pressed
glm::dvec2 g_MouseDownPT;//The position when the right mouse button is pressed

void MouseButton(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			g_MouseDown = true;
			glfwGetCursorPos(window, &g_MouseDownPT.x, &g_MouseDownPT.y);
			g_camera->startMouseRotate();
		}
		else if (action == GLFW_RELEASE) {
			g_MouseDown = false;
		}
	}

}

void CursorPosition(GLFWwindow* window, double x, double y)
{
	if (g_MouseDown) {
		g_camera->onMouseRotate((int)(x - g_MouseDownPT.x), (int)(y - g_MouseDownPT.y));
	}
}

void ScrollCallback(GLFWwindow* window, double x, double y)
{
	g_camera->zoom((int)y);
}

int main(int argc, char** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 16);

	GLFWwindow* window = glfwCreateWindow(g_WindowWidth, g_WindowHeight, "Lighting", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetWindowSizeCallback(window, SizeCallback);
	glfwSetMouseButtonCallback(window, MouseButton);
	glfwSetCursorPosCallback(window, CursorPosition);
	glfwSetScrollCallback(window, ScrollCallback);
	gl3wInit();

	g_camera = new Camera();
	g_camera->init(glm::vec3(0, 0, 0), -25.0f, 20.0f, 40.0f);

	glfwGetWindowSize(window, &g_WindowWidth, &g_WindowHeight);
	SizeCallback(window, g_WindowWidth, g_WindowHeight);

	glEnable(GL_MULTISAMPLE);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	ShadowStruct shadow = setup_shadowmap(SH_MAP_WIDTH, SH_MAP_HEIGHT);

	GLuint program = CompileShader("phong.vert", "phong.frag");
	GLuint shadow_program = CompileShader("shadow.vert", "shadow.frag");
	GLuint curve_program = CompileShader("curve.vert", "curve.frag");
	GLuint skybox_program = CompileShader("skybox.vert", "skybox.frag");
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

	Mesh::init();
	Texture::init();

	g_Curve = new Curve();
	g_Curve->create();

	Skybox* skybox = new Skybox();
	skybox->create(100.0f);

	Node* rootNode = new Node();
	Light* mainLight = Light::createDirectionalLight(glm::vec3(1, 0.5, 1), glm::vec3(0.6, 0.6, 0.6));
	rootNode->addChildNode(mainLight);

	Q* q = new Q();
	q->create();
	q->setPosition(glm::vec3(9, 2.7f, 13));
	q->setRotation(glm::vec3(90, 30, 0));
	q->setScale(glm::vec3(4, 6, 4));
	q->resetLocationMatrix();
	rootNode->addChildNode(q);

	Terrain* terrain = new Terrain();
	terrain->create(2.0f, 120, 32.0f,50);
	terrain->setRotation(glm::vec3(0, 10, 0));
	terrain->setScale(glm::vec3(1.1, 1.1, 1.1));
	terrain->resetLocationMatrix();
	rootNode->addChildNode(terrain);

 	ObjFile* objFile = new ObjFile();
	objFile->load("res/house/house.obj");
	Node* houseNode = new Node();
	houseNode->createObj(objFile);
	houseNode->setRotation(glm::vec3(0, 0, 0));
	houseNode->setScale(glm::vec3(0.01f, 0.01f, 0.01f));
	houseNode->setPosition(glm::vec3(0, 0.2, 0));
	houseNode->resetLocationMatrix();
	rootNode->addChildNode(houseNode);

	FloorLamp* floorLamp = new FloorLamp();
	g_FloorLamps[0] = floorLamp->create();
	floorLamp->setPosition(glm::vec3(-4.5, 0, 6));
	floorLamp->resetLocationMatrix();
	rootNode->addChildNode(floorLamp);
	

	floorLamp = new FloorLamp();
	g_FloorLamps[1] = floorLamp->create();
	floorLamp->setPosition(glm::vec3(-0.5f, 0, 6));
	floorLamp->resetLocationMatrix();
	rootNode->addChildNode(floorLamp);

 	g_Jeep = new Jeep();
 	g_Jeep->create();
 	rootNode->addChildNode(g_Jeep);

	rootNode->resetWorldMatrix();


	float near_plane = 1.0f, far_plane = 5000.0f;
	float viewSize = 50.0f;
	glm::mat4 lightProjection = glm::ortho(-viewSize, viewSize, -viewSize, viewSize, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(mainLight->getLightPosition()*1000.0f, glm::vec3(0,0,0), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projectedLightSpaceMatrix = lightProjection * lightView;
	double t1 = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		double t2 = glfwGetTime();
		double dt = t2 - t1;
		if (dt > 0.0f) {
			if (g_Jeep->update(dt)) {
				if (g_TraceJeep) {
					g_camera->setCenter(g_Jeep->getPosition());
				}
			}
			t1 = t2;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			g_camera->moveFrontBack(dt * 10.0f);
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			g_camera->moveFrontBack(-dt * 10.0f);
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			g_camera->moveLeftRight(dt * 10.0f);
		}
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			g_camera->moveLeftRight(-dt * 10.0f);
		}

		glClear(GL_DEPTH_BUFFER_BIT);
		glDisable(GL_CULL_FACE);
		generateDepthMap(shadow_program, shadow, projectedLightSpaceMatrix, rootNode);
		//		saveShadowMapToBitmap(shadow.Texture, SH_MAP_WIDTH, SH_MAP_HEIGHT);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, g_WindowWidth, g_WindowHeight);
		glEnable(GL_CULL_FACE);

		glUseProgram(skybox_program);
		glUniformMatrix4fv(glGetUniformLocation(skybox_program, "view"), 1, GL_FALSE, glm::value_ptr(g_camera->getViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(skybox_program, "projection"), 1, GL_FALSE, glm::value_ptr(g_camera->getProjectionMatrix()));
		glUniform3fv(glGetUniformLocation(skybox_program, "camPos"), 1, glm::value_ptr(g_camera->getPosition()));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Texture::getTexture(TEXTURE_ID_SKYBOX)->getTextureID());
		glUniform1i(glGetUniformLocation(skybox_program, "skyTexture"), 0);

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		skybox->render();
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);


		renderWithShadow(program, shadow, projectedLightSpaceMatrix,rootNode);


		glUseProgram(curve_program);
		glUniformMatrix4fv(glGetUniformLocation(curve_program, "view"), 1, GL_FALSE, glm::value_ptr(g_camera->getViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(curve_program, "projection"), 1, GL_FALSE, glm::value_ptr(g_camera->getProjectionMatrix()));

		g_Curve->render();


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	g_Jeep = NULL;
	delete rootNode;
	delete objFile;
	delete g_camera;
	delete g_Curve;
	delete skybox;
	Mesh::unInit();
	Texture::unInit();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}