#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <LearnOpenGL/assets_directory.h>
#include <LearnOpenGL/opengl_window.h>
#include <LearnOpenGL/texture_loader.h>
#include <LearnOpenGL/geometry_data.h>
#include <LearnOpenGL/imgui_helper.h>

#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace glm;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 550;
const string localShaderDir("AdvanceOpenGL/advance_data_and_glsl/");

// camera
Camera camera(glm::vec3(0.0f, 0.1f, 3.0f));

int selection = 0;

void drawGUI();


int main() {
	OpenGLWindow window; {
		try { window.initWindow(SCR_WIDTH, SCR_HEIGHT, "AdvanceOpenGL_AdvanceDataAndGLSL"); }
		catch(OpenGLWindowException e) {
			cout << e.what() << endl;
			return -1;
		}
		
		window.setCursorPosCallback([](GLFWwindow* pw, double xpos, double ypos) {
			if (camera.firstMouse) {
				OpenGLWindow::lastX = xpos;
				OpenGLWindow::lastY = ypos;
				camera.firstMouse = false;
			}
			int xoffset, yoffset;
			xoffset = xpos - OpenGLWindow::lastX;
			yoffset = OpenGLWindow::lastY - ypos;
			OpenGLWindow::lastX = xpos;
			OpenGLWindow::lastY = ypos;
			camera.ProcessMouseMovement(xoffset, yoffset);
		});
		window.setInputProcessor([](const OpenGLWindow& window) {
			if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose(&window, true);}
			if (glfwGetKey(&window, GLFW_KEY_W) == GLFW_PRESS) {
				camera.ProcessKeyboard(Camera_Movement::FORWARD, OpenGLWindow::deltaTime);}
			if (glfwGetKey(&window, GLFW_KEY_S) == GLFW_PRESS) {
				camera.ProcessKeyboard(Camera_Movement::BACKWARD, OpenGLWindow::deltaTime);}
			if (glfwGetKey(&window, GLFW_KEY_A) == GLFW_PRESS) {
				camera.ProcessKeyboard(Camera_Movement::LEFT, OpenGLWindow::deltaTime);}
			if (glfwGetKey(&window, GLFW_KEY_D) == GLFW_PRESS) {
				camera.ProcessKeyboard(Camera_Movement::RIGHT, OpenGLWindow::deltaTime);}
			if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
				if (!camera.mouse_ctrl) {
					window.setCursorDisable();
					camera.mouse_ctrl = true;
				}}
			if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE) {
				if (camera.mouse_ctrl) {
					window.setCursorEnable();
					camera.mouse_ctrl = false;
				}}
		});
		ImGuiHelper::initImGui(&window);
		//imgui 1.60的bug，在设置滚轮回调之前要先初始化imgui，否则滚轮事件没有回调
		window.setScrollCallback([](GLFWwindow* pw, double xoff, double yoff) {
			camera.ProcessMouseScroll(yoff);
		});
	}

	glEnable(GL_DEPTH_TEST);

	vector<string> face_paths{ "right.jpg","left.jpg","top.jpg","bottom.jpg","front.jpg","back.jpg" };
	for (int i = 0; i < face_paths.size(); i++)
		face_paths[i] = dir_textures + "skybox/" + face_paths[i];
	TextureLoader skyboxTexture(face_paths);

	TextureLoader frontTexture(dir_textures + "container.jpg",0);
	TextureLoader backTexture(dir_textures + "wall.jpg",1);
	Shader shader(dir_shaders + localShaderDir + "advanceGLSL.vs", dir_shaders + localShaderDir + "advanceGLSL.fs");
	Shader skyboxShader(dir_shaders + "AdvanceOpenGL/cubemap/skyboxShader.vs", dir_shaders + "AdvanceOpenGL/cubemap/skyboxShader.fs");
	CubeData cubeData;
	Mesh box(cubeData.vertices, cubeData.indices, cubeData.textures);

	//使用uniform buffer object 的例子
	//GLuint uniformBlockIndex = glGetUniformBlockIndex(shader.ID, "Matrices");
	//glUniformBlockBinding(shader.ID, uniformBlockIndex,0);
	//GLuint UBOMatrices;
	//glGenBuffers(1, &UBOMatrices);
	//glBindBuffer(GL_UNIFORM_BUFFER, UBOMatrices);
	//glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBOMatrices, 0, 2 * sizeof(glm::mat4));
	//mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	//glBindBuffer(GL_UNIFORM_BUFFER, UBOMatrices);
	//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//封装之后的ubo
	UniformBuffer ubo(shader.ID, "Matrices",0,2*sizeof(mat4));
	mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	ubo.setSubData(sizeof(mat4), sizeof(mat4), value_ptr(projection));

	while (!window.isWindowClosed()) {
		OpenGLWindow::calculateDeltaTime();
		window.processInput();
		glClearColor(0.0, 0.0, 0.5, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mat4 view = camera.GetViewMatrix();
		ubo.setSubData(0, sizeof(mat4), value_ptr(view));
		/*glBindBuffer(GL_UNIFORM_BUFFER, UBOMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);*/

		mat4 model = mat4(1.0f);
		shader.use();
		shader.setMat4("model", model);
		shader.setFloat("pointSize", 10);
		shader.setInt("selection", selection);
		shader.setInt("frontTexture", frontTexture.getTextureUnit());
		shader.setInt("backTexture", backTexture.getTextureUnit());

		switch (selection) {
		case 0://gl_PointSize
			glEnable(GL_PROGRAM_POINT_SIZE);
			box.drawPoint(shader);
			glDisable(GL_PROGRAM_POINT_SIZE);
			break;
		//gl_VertexId
		case 1://gl_FragCoord
			box.Draw(shader);
			break;
		case 2: //gl_FrontFace
			frontTexture.activeAndBind();
			backTexture.activeAndBind();
			box.Draw(shader);
			frontTexture.unactiveAndUnbind();
			backTexture.unactiveAndUnbind();
			break;
		}

		/*Draw skybox*/ {
			glDepthFunc(GL_LEQUAL);
			skyboxShader.use();
			view = mat4(mat3(camera.GetViewMatrix())); // remove translation from the view matrix
			skyboxShader.setMat4("view", view);
			skyboxShader.setMat4("projection", projection);
			box.Draw(skyboxShader);
			glDepthFunc(GL_LESS);
		}

		ImGuiHelper::drawImGui(drawGUI);
		window.swapBuffersAndPollEvents();
	}

	return 0;
}


void drawGUI() {
	static const char* selectionNames[3] = { "drawPoint(gl_PointSize)","drawCube(gl_FragCood)","drawCube(gl_FrontFacing)" };
	ImGui::Begin("ToolBox");
	if (ImGui::TreeNode("Selection")) {
		for (int n = 0; n < _countof(selectionNames); n++) {
			if (ImGui::Selectable(selectionNames[n], selection == n)) { 
				selection = n; }
		}
		ImGui::TreePop();
	}
	float frameRate = ImGui::GetIO().Framerate;
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f /frameRate , frameRate);
	ImGui::End();
}
