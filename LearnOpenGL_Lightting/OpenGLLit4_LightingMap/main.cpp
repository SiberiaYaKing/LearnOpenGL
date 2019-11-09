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

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#include <texture.h>

using namespace std;
using namespace glm;

//cube with normal
float vertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

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

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;
bool firstMouse = true;
Camera myCam(vec3(0, 0, 3));

inline void processInput(OpenGLWindow &);
//inline int LoadTexture(const char* img_path);

static float shininess = 0.25f;
ImVec4 la(0.2f, 0.2f, 0.2f,1);
ImVec4 ld(0.5f, 0.5f, 0.5f,1);
ImVec4 ls(1.0f, 1.0f, 1.0f,1);

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

	////=======================================
	//unsigned emissionMap = LoadTexture("matrix.jpg");
	//unsigned diffuseMap = LoadTexture("container2.png");
	//unsigned specularMap = LoadTexture("lighting_maps_specular_color.png");
	//unsigned specularMap = LoadTexture("container2_specular.png");
	////===========================================

	Texture emissiomMap,diffuseMap,specularMap;
	diffuseMap.initTexture("container2.png");
	emissiomMap.initTexture("matrix.jpg"); 
	specularMap.initTexture("container2_specular.png");
	//emissiomMap.SetFiltering(GL_NEAREST);
	//emissiomMap.SetMipmapFiltering(GL_LINEAR_MIPMAP_NEAREST);
	//emissiomMap.SetWrapping(GL_CLAMP_TO_EDGE);
	
	glEnable(GL_DEPTH_TEST);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window.getWindowPtr(), true);
	ImGui::StyleColorsDark();
	bool tool_active = true;
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

		//===========================================
		/*glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);*/
		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, emissionMap);
		//=============================================
		diffuseMap.ActiveAndBind(GL_TEXTURE0);
		specularMap.ActiveAndBind(GL_TEXTURE1);
		emissiomMap.ActiveAndBind(GL_TEXTURE2);


		lightingShader.use();
		lightingShader.setVec3("viewPos", myCam.Position);
		//matrix
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("model", model);
		lightingShader.setMat3("normalMat", normalMat);
		//material
		lightingShader.setInt("material.diffuse", 0);
		lightingShader.setInt("material.specular",1);
		lightingShader.setInt("material.emission", 2);
		lightingShader.setFloat("material.shininess",shininess*128.0f);
		//light
		lightingShader.setVec3("light.position", lightPos);
		lightingShader.setVec3("light.ambient", la.x,la.y,la.z);
		lightingShader.setVec3("light.diffuse", ld.x,ld.y,ld.z); // 将光照调暗了一些以搭配场景
		lightingShader.setVec3("light.specular",ls.x,ls.y,ls.z);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = mat4(1.0f);  //清空model为单位阵
		model = translate(model, lightPos);
		model = scale(model, vec3(0.2f)); // a smaller cube

		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		lampShader.setMat4("model", model);
		lampShader.setVec3("lightDiff", ls.x,ls.y,ls.z);
		//lampShader.setVec3("lightDiff", lc); 
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		ImGui_ImplGlfwGL3_NewFrame();
		{
			ImGui::Begin("Tools", &tool_active);
			static float yaw = -55;
			static float pitch = 28;
			static float zoom = 45.0f;
			static vec3 position{ -2.8f,-2.0f,5.6f };
			ImGui::Text("Camera Parama");                        
			ImGui::SliderFloat("Yaw", &yaw, -180, 180);
			ImGui::SliderFloat("Pitch", &pitch, -89.0f, 89.0f);
			myCam.SetRotation(yaw, pitch);
			ImGui::DragFloat3("Position", value_ptr(position),0.05f);
			myCam.SetPosition(position);
			ImGui::SliderFloat("Zoom", &zoom, 0.0f, 60.0f);
			myCam.SetZoom(zoom);
			ImGui::Text("Light Color");
			ImGui::ColorEdit3("light ambient", (float*)&la);
			ImGui::ColorEdit3("light diffuse", (float*)&ld); 
			ImGui::ColorEdit3("light specular", (float*)&ls);
			ImGui::Text("Material Color");
			ImGui::SliderFloat("mat shininess", &shininess, 0.001f, 1);
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

//int LoadTexture(const char* img_path) {
//	unsigned int texture;
//	glGenTextures(1, &texture);
//	glBindTexture(GL_TEXTURE_2D, texture);
//
//	int width, height, nrChannels;
//	unsigned char *data = stbi_load(img_path, &width, &height, &nrChannels,0);
//	if (data) {
//		switch (nrChannels){
//		case 3:
//			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//			break;
//		case 4:
//			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//			break;
//		}
//		glGenerateMipmap(GL_TEXTURE_2D);
//	}else {
//		cout << "Failed to load texture" << endl;
//	}
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//
//	stbi_image_free(data);
//	return texture;
//}

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

