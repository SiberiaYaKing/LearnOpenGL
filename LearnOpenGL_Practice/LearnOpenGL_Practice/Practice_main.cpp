#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui\imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#include <LearnOpenGL/shader.h>
#include <LearnOpenGL/camera.h>
#include <LearnOpenGL/opengl_window.h>
#include <LearnOpenGL/texture_loader.h>

#include <iostream>

using namespace glm;
using namespace std;

void processInput(GLFWwindow *window);
void DrawGUI();

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float cubeVertices[] = {
	// positions          // texture Coords
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};


vec3 gizmoPos(0.5f);
vec2 scrPos;


int main() {
	// window init -------------
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
	window.setScrollCallback([](GLFWwindow* window, double xoffset, double yoffset) {
		camera.ProcessMouseScroll(yoffset);
	});
	//---------------------

	glEnable(GL_DEPTH_TEST);

	//cubeShader and texture init ----------------
	Shader cubeShader("cubeShader.vs","cubeShader.fs");
	TextureLoader cubeTex("container.jpg");
	TextureLoader gizmoTex("gizmo.png");
	cubeShader.use();
	cubeShader.setInt("texture1", 0);
	//----------------------

	//vertex data init -----------------------
	unsigned cubeVBO, cubeVAO;
	glGenBuffers(1, &cubeVBO);
	glGenVertexArrays(1, &cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(0);
	//---------------------------


	//init GUI--------------
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(&window, true);
	ImGui::StyleColorsDark();
	//-----------------------------

	while (!window.isWindowClosed()) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		processInput(&window);

		//Draw big cube  ---------------------
		mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		mat4 view = camera.GetViewMatrix();
		mat4 model = mat4(1.0f);
		cubeShader.use();
		cubeShader.setMat4("projection", projection);
		cubeShader.setMat4("view", view);
		cubeShader.setMat4("model", model);
		glBindVertexArray(cubeVAO);
		TextureLoader::activeTexture(GL_TEXTURE0);
		cubeTex.bindTexture();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		//------------------------------

		//Draw little red cube(gizmo)---------------------
		model = translate(model, gizmoPos);
		model = scale(model, vec3(0.02f));
		cubeShader.setMat4("model", model);
		glBindVertexArray(cubeVAO);
		TextureLoader::activeTexture(GL_TEXTURE0);
		gizmoTex.bindTexture();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		//--------------------------------------

		//将小红块的坐标从世界空间转换到裁剪空间
		vec4 clipPos = projection * view*model*vec4(gizmoPos, 1.0f);
		//齐次除法&屏幕坐标映射
		float x = (clipPos.x / clipPos.w + 1)*0.5f*SCR_WIDTH;
		float y = (clipPos.y / clipPos.w - 1)*-0.5f*SCR_HEIGHT;
		scrPos = vec2(x, y);
		////输出屏幕坐标
		//if (x >= 0 && x <= SCR_WIDTH && y >= 0 && y <= SCR_HEIGHT)
		//	cout << "x: " << scrPos.x << " y: " << scrPos.y << endl;
		//else cout << "gizmo out of screen range" << endl;
		
		DrawGUI();

		window.swapBuffersAndPollEvents();
	}

	//Close GUI
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

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

inline void DrawGUI() {
	ImGui_ImplGlfwGL3_NewFrame();
	{
		ImGui::Begin("Screen Postion");
		float arr[2] = { scrPos.x,scrPos.y };
		if (arr[0] >= 0 && arr[0] <= SCR_WIDTH && arr[1] >= 0 && arr[1] <= SCR_HEIGHT)
			ImGui::InputFloat2("Target ScrPos", arr);
		else ImGui::Text("Target out of screen");
		ImGui::Text("(Target is the little red cube(gizmo)");
		ImGui::Text(" of the big cube's corner)");
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}