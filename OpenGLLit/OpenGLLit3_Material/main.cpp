//gl相关的头文件有顺序要求

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shader.h>
#include <camera.h>
#include <opengl_window.h>

#include <iostream>
#include <math.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"


using namespace std;
using namespace glm;

//cube with normal
float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;
bool firstMouse = true;
Camera myCam(vec3(0, 0, 3));

inline void processInput(GLWindow &);



static float intencity = 1.0f;
static float shininess = 0.25f;
//ImVec4 ma(1.0f, 0.5f, 0.31f,1);
//ImVec4 md(1.0f, 0.5f, 0.31f,1);
//ImVec4 ms(0.5f, 0.5f, 0.5f,1);
//ImVec4 la(0.2f, 0.2f, 0.2f,1);
//ImVec4 ld(0.5f, 0.5f, 0.5f,1);
//ImVec4 ls(1.0f, 1.0f, 1.0f,1);
ImVec4 ma(0.0f, 0.1f, 0.06f, 1);
ImVec4 md(0.0f, 0.50980392f, 0.50980392f, 1);
ImVec4 ms(0.50196078f, 0.50196078f, 0.50196078f, 1);
ImVec4 lightColor(1.0f, 1, 1, 1);
//ImVec4 la(1.0f, 1.0f, 1.0f, 1);
//ImVec4 ld(1.0f, 1.0f, 1.0f, 1);
//ImVec4 ls(1.0f, 1.0f, 1.0f, 1);


int main() {
	
	GLWindow window;
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//light-obj vertices process
	glGenVertexArrays(1, &lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindVertexArray(lightVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);

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
		mat3 normalMat = mat3(transpose(inverse(model)));  //法线矩阵
		vec3 lightPos = vec3(sin(glfwGetTime()), 1.6f, 2.6f);
		vec3 lc(lightColor.x*intencity, lightColor.y*intencity, lightColor.z*intencity);

		lightingShader.use();
		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("viewPos", myCam.Position);
		//matrix
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("model", model);
		lightingShader.setMat3("normalMat", normalMat);
		//material
		lightingShader.setVec3("material.ambient", ma.x,ma.y,ma.z);
		lightingShader.setVec3("material.diffuse", md.x,md.y,md.z);
		lightingShader.setVec3("material.specular", ms.x,ms.y,ms.z);
		lightingShader.setFloat("material.shininess",shininess*128.0f);
		//light
		//lightingShader.setVec3("light.ambient", la.x,la.y,la.z);
		//lightingShader.setVec3("light.diffuse", ld.x,ld.y,ld.z); // 将光照调暗了一些以搭配场景
		//lightingShader.setVec3("light.specular",ls.x,ls.y,ls.z);
		lightingShader.setVec3("lightPos", lightPos);
		lightingShader.setVec3("lightColor", lc);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = mat4(1.0f);  //清空model为单位阵
		model = translate(model, lightPos);
		model = scale(model, vec3(0.2f)); // a smaller cube

		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		lampShader.setMat4("model", model);
		lampShader.setVec3("lightDiff", lc); /*la.x*(ls.x)*ld.x,la.y*(ls.y)*ld.y, la.z*(ls.z)*ld.z);*/
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		ImGui_ImplGlfwGL3_NewFrame();
		{
			static float yaw = -55;
			static float pitch = 28;
			static vec3 position{ -2.8f,-2.0f,5.6f };
			ImGui::Text("Camera Parama");                        
			ImGui::SliderFloat("Yaw", &yaw, -180, 180);
			ImGui::SliderFloat("Pitch", &pitch, -89.0f, 89.0f);
			myCam.SetRotation(yaw, pitch);
			ImGui::DragFloat3("Position", value_ptr(position),0.05f);
			myCam.SetPosition(position);
			ImGui::Text("Light Color");
			ImGui::DragFloat("intencity", &intencity,0.05f,0,5);
			ImGui::ColorEdit3("lightColor", (float*)&lightColor);
			//ImGui::ColorEdit3("light ambient", (float*)&la);
			//ImGui::ColorEdit3("light diffuse", (float*)&ld); 
			//ImGui::ColorEdit3("light specular", (float*)&ls);
			ImGui::Text("Material Color");
			ImGui::ColorEdit3("mat ambient", (float*)&ma);
			ImGui::ColorEdit3("mat diffuse", (float*)&md);
			ImGui::ColorEdit3("mat specular", (float*)&ms);
			ImGui::SliderFloat("mat shininess", &shininess, 0, 1);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
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

void processInput(GLWindow & w)
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

