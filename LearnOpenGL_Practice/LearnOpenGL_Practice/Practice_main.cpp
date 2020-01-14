#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui\imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#include <LearnOpenGL/shader.h>
#include <LearnOpenGL/camera.h>
#include <LearnOpenGL/model.h>
#include <LearnOpenGL/assets_directory.h>
#include <LearnOpenGL/opengl_window.h>
#include <LearnOpenGL/texture_loader.h>

#include <iostream>

using namespace glm;
using namespace std;

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float vertices[] = {
	// positions          // normals           // texture coords
	 0.01f, -0.01f, -0.01f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	-0.01f, -0.01f, -0.01f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	 0.01f,  0.01f, -0.01f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	 0.01f,  0.01f, -0.01f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-0.01f, -0.01f, -0.01f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	-0.01f,  0.01f, -0.01f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,


	-0.01f, -0.01f,  0.01f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	 0.01f, -0.01f,  0.01f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	 0.01f,  0.01f,  0.01f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	 0.01f,  0.01f,  0.01f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	-0.01f,  0.01f,  0.01f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	-0.01f, -0.01f,  0.01f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	-0.01f,  0.01f,  0.01f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	-0.01f,  0.01f, -0.01f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	-0.01f, -0.01f, -0.01f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.01f, -0.01f, -0.01f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.01f, -0.01f,  0.01f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	-0.01f,  0.01f,  0.01f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	 0.01f,  0.01f, -0.01f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	 0.01f,  0.01f,  0.01f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 0.01f, -0.01f, -0.01f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.01f, -0.01f, -0.01f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.01f,  0.01f,  0.01f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 0.01f, -0.01f,  0.01f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

	-0.01f, -0.01f, -0.01f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	 0.01f, -0.01f, -0.01f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	 0.01f, -0.01f,  0.01f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	 0.01f, -0.01f,  0.01f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-0.01f, -0.01f,  0.01f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-0.01f, -0.01f, -0.01f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	 0.01f,  0.01f, -0.01f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	-0.01f,  0.01f, -0.01f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	 0.01f,  0.01f,  0.01f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	 0.01f,  0.01f,  0.01f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-0.01f,  0.01f, -0.01f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	-0.01f,  0.01f,  0.01f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f
};

vec3 cubePos(0.0f);
vec2 scrPos;

int main() {
	OpenGLWindow window;
	try {
		window.initWindow(SCR_WIDTH, SCR_HEIGHT, "Practice");
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
		return -1;
	}
	window.setCursorDisable();
	window.setCursorPosCallback([](GLFWwindow* window, double xpos, double ypos) {
		if (camera.firstMouse) {
			lastX = xpos;
			lastY = ypos;
			camera.firstMouse = false;
		}
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos, lastY = ypos;
		camera.ProcessMouseMovement(xoffset, yoffset);
	});
	//window.setScrollCallback([](GLFWwindow* window, double xoffset, double yoffset) {
	//	camera.ProcessMouseScroll(yoffset);
	//});

	glEnable(GL_DEPTH_TEST);

	Shader shader(dir_shaders+"lampShader.vs",dir_shaders+"lampShader.fs");

	unsigned cubeVBO, cubeVAO;
	glGenBuffers(1, &cubeVBO);
	glGenVertexArrays(1, &cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//ImGui::CreateContext();
	//ImGui_ImplGlfwGL3_Init(window.getWindowPtr(), true);
	//ImGui::StyleColorsDark();
	////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	while (!window.isWindowClosed()) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		processInput(&window);

		mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		mat4 view = camera.GetViewMatrix();
		mat4 model = mat4(1.0f);
		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);

		vec4 clipPos = projection * view*model*vec4(cubePos, 1.0f);
		scrPos = vec2((clipPos.x / clipPos.w+1)*0.5f*SCR_WIDTH, 
			(clipPos.y / clipPos.w-1)*-0.5f*SCR_HEIGHT);

		cout << "x: " << scrPos.x << " y: " << scrPos.y<< endl;

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		window.swapBuffersAndPollEvents();
	}

	glDeleteVertexArrays(1,&cubeVAO);
	glDeleteBuffers(1,&cubeVBO);
	return 0;

}

void processInput(GLFWwindow * window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}
