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
bool firstMouse = true;
Camera myCam(vec3(0, 0, 3));

inline void processInput(OpenGLWindow &);

float shininess = 0.25f;
vec4 la(0.2f, 0.2f, 0.2f,1);
vec4 ld(0.5f, 0.5f, 0.5f,1);
vec4 ls(1.0f, 1.0f, 1.0f,1);

//vec3 lightPos = vec3(1.0f, 1.6f, 2.6f);

int main() {
	
	OpenGLWindow window;
	try {
		window.initWindow(SCR_WIDTH,SCR_HEIGHT,"OpenGL_Lighting");
	}
	catch (exception e) {
		cout<<e.what()<<endl;
		return -1;
	}
	window.setCursorDisable();
	window.setCursorPosCallback([](GLFWwindow* window, double xpos, double ypos) {
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;
		myCam.ProcessMouseMovement(xoffset, yoffset);
	});
	window.setScrollCallback([](GLFWwindow* window, double xoffset, double yoffset) {
		myCam.ProcessMouseScroll(yoffset);
	});

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

	Texture emissiomMap,diffuseMap,specularMap;
	diffuseMap.initTexture("container2.png"); 
	specularMap.initTexture("container2_specular.png");
	
	glEnable(GL_DEPTH_TEST);

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

		diffuseMap.ActiveAndBind(GL_TEXTURE0);
		specularMap.ActiveAndBind(GL_TEXTURE1);

		lightingShader.use();
		lightingShader.setVec3("viewPos", myCam.Position);
		//matrix
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		//lightingShader.setMat4("model", model);
		lightingShader.setMat3("normalMat", normalMat);
		//material
		lightingShader.setInt("material.diffuse", 0);
		lightingShader.setInt("material.specular",1);
		lightingShader.setFloat("material.shininess",shininess*128.0f);
		//directionalLight
		lightingShader.setVec3("directionalLight.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("directionalLight.ambient", la.x,la.y,la.z);
		lightingShader.setVec3("directionalLight.diffuse", ld.x,ld.y,ld.z);
		lightingShader.setVec3("directionalLight.specular",ls.x,ls.y,ls.z);
		glBindVertexArray(cubeVAO);
		for (unsigned int i = 0; i < 10; i++) {
			model = mat4(1.0f);
			model = translate(model, cubePositions[i]);
			float angle = 20.0;
			//quat q;
			//q= rotate(q, radians(angle), vec3(1.0f, 0.3f, 0.5f));
			//model = model * mat4_cast(q);
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		//model = mat4(1.0f);  //清空model为单位阵
		//model = translate(model, lightPos);
		//model = scale(model, vec3(0.2f)); // a smaller cube

		//lampShader.use();
		//lampShader.setMat4("projection", projection);
		//lampShader.setMat4("view", view);
		//lampShader.setMat4("model", model);
		//lampShader.setVec3("lightDiff", ls.x,ls.y,ls.z);
		//glBindVertexArray(lightVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		window.swapBuffersAndPollEvents();
	}


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

