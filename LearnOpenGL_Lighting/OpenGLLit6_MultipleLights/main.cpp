//gl相关的头文件有顺序要求

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shader.h>
#include <camera.h>
#include <opengl_window.h>
#include <string>

#include <iostream>
#include <math.h>
#include <string>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#include <texture.h>

using namespace std;
using namespace glm;

//cube with normal !!true cull
float vertices[] = {
	// positions          // normals           // texture coords
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,


	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f
};

//10个立方体的位置
vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;
Camera myCam(vec3(0, 0, 3));

inline void processInput(OpenGLWindow &);

float shininess = 0.25f;

//DirectionLight
bool isDirLight = true;
vec4 dla(0.05f, 0.05f, 0.05f, 1);
vec4 dld(0.4f, 0.4f, 0.4f, 1);
vec4 dls(0.5f, 0.5f, 0.5f, 1);

//PointLight
bool isPointLight = true;
vec4 pla(0.05f, 0.05f, 0.05f,1);
vec4 pld(0.8f, 0.8f, 0.8f,1);
vec4 pls(1.0f, 1.0f, 1.0f,1);

//SpotLight
bool isSpotLight = true;
vec4 sla(0.0f, 0.0f, 0.0f, 1);
vec4 sld(1.0f, 1.0f, 1.0f, 1);
vec4 sls(1.0f, 1.0f, 1.0f, 1);


glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};


int main() {
	
	OpenGLWindow window;
	try {
		window.initWindow(SCR_WIDTH,SCR_HEIGHT,"OpenGL_Lighting");
	}
	catch (exception e) {
		cout<<e.what()<<endl;
		return -1;
	}

	Shader lightingShader("lightingShader.vs", "lightingShader.fs");
	Shader lampShader("lampShader.vs", "lampShader.fs");

	unsigned cubeVAO, VBO,lightVAO;
	glGenBuffers(1, &VBO);
	//box vertices process
	glGenVertexArrays(1, &cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//light-obj vertices process
	glGenVertexArrays(1, &lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindVertexArray(lightVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	Texture emissiomMap,diffuseMap,specularMap,filmMap;
	diffuseMap.initTexture("container2.png"); 
	specularMap.initTexture("container2_specular.png");
	filmMap.initTexture("matrix.jpg");
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window.getWindowPtr(), true);
	ImGui::StyleColorsDark();
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	while (!window.isWindowClosed()) {

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		processInput(window);

		mat4 projection = perspective(radians(myCam.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		mat4 view = myCam.GetViewMatrix();
		mat4 model = mat4(1.0f);

		diffuseMap.ActiveAndBind(GL_TEXTURE0);
		specularMap.ActiveAndBind(GL_TEXTURE1);
		filmMap.ActiveAndBind(GL_TEXTURE2);

		lightingShader.use();
		lightingShader.setVec3("viewPos", myCam.Position);
		//matrix
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("model", model);
		//material
		lightingShader.setInt("material.diffuse", 0);
		lightingShader.setInt("material.specular",1);
		lightingShader.setInt("material.film", 2);
		lightingShader.setFloat("material.shininess",shininess*128.0f);

		////directionalLight
		vec3 lightDir(-0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("dirLight.direction", lightDir);
		if (isDirLight) {
			lightingShader.setVec3("dirLight.ambient", dla.x, dla.y, dla.z);
			lightingShader.setVec3("dirLight.diffuse", dld.x, dld.y, dld.z);
			lightingShader.setVec3("dirLight.specular", dls.x, dls.y, dls.z);
		}
		else {
			lightingShader.setVec3("dirLight.ambient", vec3(0));
			lightingShader.setVec3("dirLight.diffuse", vec3(0));
			lightingShader.setVec3("dirLight.specular", vec3(0));
		}

		//spotLight
		float phi = 12.5f, gamma = 17.5f;
		lightingShader.setVec3("spotLight.position", myCam.Position);
		lightingShader.setVec3("spotLight.direction", myCam.Front);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(phi)));
		lightingShader.setFloat("spotLight.outterCutOff", glm::cos(glm::radians(gamma)));
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.09f);
		lightingShader.setFloat("spotLight.quadratic", 0.032f);
		if (isSpotLight) {
			lightingShader.setVec3("spotLight.ambient", sla.x, sla.y, sla.z);
			lightingShader.setVec3("spotLight.diffuse", sld.x, sld.y, sld.z);
			lightingShader.setVec3("spotLight.specular", sls.x, sls.y, sls.z);
		}
		else {
			lightingShader.setVec3("spotLight.ambient", vec3(0));
			lightingShader.setVec3("spotLight.diffuse", vec3(0));
			lightingShader.setVec3("spotLight.specular", vec3(0));
		}



		for (int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); i++) {
			string lightName("pointLights");
			char c = i + '0';
			lightName = lightName + '[' + c + ']';
			//pointLight
			lightingShader.setVec3(lightName + ".position", pointLightPositions[i]);
			if (isPointLight) {
				lightingShader.setVec3(lightName + ".ambient", pla.x, pla.y, pla.z);
				lightingShader.setVec3(lightName + ".diffuse", pld.x, pld.y, pld.z);
				lightingShader.setVec3(lightName + ".specular", pls.x, pls.y, pls.z);
			}
			else {
				lightingShader.setVec3(lightName + ".ambient", vec3(0));
				lightingShader.setVec3(lightName + ".diffuse", vec3(0));
				lightingShader.setVec3(lightName + ".specular", vec3(0));
			}
			lightingShader.setFloat(lightName + ".constant", 1.0f);
			lightingShader.setFloat(lightName + ".linear", 0.09f);
			lightingShader.setFloat(lightName + ".quadratic", 0.032f);
		}

		glBindVertexArray(cubeVAO);
		for (unsigned int i = 0; i < 10; i++) {
			model = mat4(1.0f);
			model = translate(model, cubePositions[i]);
			float angle = 20.0*i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			//坑!!!法线矩阵需要和模型矩阵同时计算
			//mat3 normalMat = mat3(transpose(inverse(model)));
			//lightingShader.setMat3("normalMat", normalMat);
			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		for (int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]) && isPointLight; i++) {
			//pointLight
			model = mat4(1.0f);  //清空model为单位阵
			model = translate(model, pointLightPositions[i]);
			model = scale(model, vec3(0.2f)); // a smaller cube
			lampShader.use();
			lampShader.setMat4("projection", projection);
			lampShader.setMat4("view", view);
			lampShader.setMat4("model", model);
			lampShader.setVec3("lightDiff", pls.x, pls.y, pls.z);
			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		ImGui_ImplGlfwGL3_NewFrame();
		{
			ImGui::Begin("Tools");
			static float yaw = -90;
			static float pitch = 0.0f;
			static float zoom = 45.0f;
			//static vec3 position{2.15f,0.7f,3.5f};
			ImGui::Text("Camera Parama");
			ImGui::SliderFloat("Yaw", &yaw, -180, 180);
			ImGui::SliderFloat("Pitch", &pitch, -89.0f, 89.0f);
			myCam.SetRotation(yaw, pitch);
			ImGui::Text("Use 'WSAD' to control the Camera movement");
			//ImGui::DragFloat3("Position", value_ptr(position), 0.05f);
			//myCam.SetPosition(position);
			ImGui::SliderFloat("Zoom", &zoom, 0.0f, 60.0f);
			myCam.SetZoom(zoom);
			ImGui::Text("Light Selecter");
			ImGui::Checkbox("Open Direction Light", &isDirLight);
			ImGui::Checkbox("Open Point Light",&isPointLight);
			ImGui::Checkbox("Open Spot Light", &isSpotLight);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		window.swapBuffersAndPollEvents();
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	return 0;
}

void processInput(OpenGLWindow & w)
{
	GLFWwindow *pw = w.getWindowPtr();
	if (glfwGetKey(pw, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(pw, true);
	if (glfwGetKey(pw, GLFW_KEY_W) == GLFW_PRESS)
		myCam.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(pw, GLFW_KEY_S) == GLFW_PRESS)
		myCam.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(pw, GLFW_KEY_A) == GLFW_PRESS)
		myCam.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(pw, GLFW_KEY_D) == GLFW_PRESS)
		myCam.ProcessKeyboard(RIGHT, deltaTime);
}
