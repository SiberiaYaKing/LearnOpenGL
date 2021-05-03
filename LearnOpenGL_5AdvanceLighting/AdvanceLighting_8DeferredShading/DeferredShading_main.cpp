#include <pch.h>

using namespace std;
using namespace glm;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 560;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
const string localShaderDir = dir_shaders + ("AdvanceLighting/DeferredShading/");

int selection = 0;

void drawGUI();

int main() {
	OpenGLWindow window; {
		try { window.initWindow(SCR_WIDTH, SCR_HEIGHT, "AdvanceLighting_DeferredShading"); }
		catch (OpenGLWindowException e) {
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
				glfwSetWindowShouldClose(&window, true);
			}
			if (glfwGetKey(&window, GLFW_KEY_W) == GLFW_PRESS) {
				camera.ProcessKeyboard(Camera_Movement::FORWARD, OpenGLWindow::deltaTime);
			}
			if (glfwGetKey(&window, GLFW_KEY_S) == GLFW_PRESS) {
				camera.ProcessKeyboard(Camera_Movement::BACKWARD, OpenGLWindow::deltaTime);
			}
			if (glfwGetKey(&window, GLFW_KEY_A) == GLFW_PRESS) {
				camera.ProcessKeyboard(Camera_Movement::LEFT, OpenGLWindow::deltaTime);
			}
			if (glfwGetKey(&window, GLFW_KEY_D) == GLFW_PRESS) {
				camera.ProcessKeyboard(Camera_Movement::RIGHT, OpenGLWindow::deltaTime);
			}
			if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
				if (!camera.mouse_ctrl) {
					window.setCursorDisable();
					camera.mouse_ctrl = true;
				}
			}
			if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE) {
				if (camera.mouse_ctrl) {
					window.setCursorEnable();
					camera.mouse_ctrl = false;
				}
			}
		});
		ImGuiHelper::initImGui(&window);
		//imgui 1.60的bug，在设置滚轮回调之前要先初始化imgui，否则滚轮事件没有回调
		window.setScrollCallback([](GLFWwindow* pw, double xoff, double yoff) {
			camera.ProcessMouseScroll(yoff);
		});
	}

	mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	// ========================设置渲染对象=========================
	Framebuffer gBuffer(SCR_WIDTH, SCR_HEIGHT,0);
	// position buffer
	gBuffer.attachBuffer(SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB16F, GL_RGB, GL_FLOAT);
	// normal buffer
	gBuffer.attachBuffer(SCR_WIDTH, SCR_HEIGHT, 1, GL_RGB16F, GL_RGB, GL_FLOAT);
	// albedo + specular buffer
	gBuffer.attachBuffer(SCR_WIDTH, SCR_HEIGHT, 2, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	Shader shaderGeometryPass(localShaderDir + "gBufferShader.vs", localShaderDir + "gBufferShader.fs");
	Model nanosuit(dir_models + "nanosuit/nanosuit.obj");
	vector<glm::vec3> objectPositions;
	objectPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
	objectPositions.push_back(glm::vec3(0.0, -3.0, -3.0));
	objectPositions.push_back(glm::vec3(3.0, -3.0, -3.0));
	objectPositions.push_back(glm::vec3(-3.0, -3.0, 0.0));
	objectPositions.push_back(glm::vec3(0.0, -3.0, 0.0));
	objectPositions.push_back(glm::vec3(3.0, -3.0, 0.0));
	objectPositions.push_back(glm::vec3(-3.0, -3.0, 3.0));
	objectPositions.push_back(glm::vec3(0.0, -3.0, 3.0));
	objectPositions.push_back(glm::vec3(3.0, -3.0, 3.0));

	Shader showGBufferShader(dir_shaders + "AdvanceOpenGL/frame_buffer/frame_buffer.vs", localShaderDir+"showGBuffer.fs");
	showGBufferShader.use();
	showGBufferShader.setInt("positionTex", 0);
	showGBufferShader.setInt("normalTex", 1);
	showGBufferShader.setInt("albedoSpecTex", 2);
	// ============================================================

	/* ==============Set up skybox=============== */ 
	//vector<string> face_paths{ "right.jpg","left.jpg","top.jpg","bottom.jpg","front.jpg","back.jpg" };
	//for (int i = 0; i < face_paths.size(); i++) {face_paths[i] = dir_textures + "skybox/" + face_paths[i];}
	//TextureLoader skyboxTexture(face_paths);
	//Shader skyboxShader(dir_shaders + "AdvanceOpenGL/cubemap/skyboxShader.vs", dir_shaders + "AdvanceOpenGL/cubemap/skyboxShader.fs");
	//CubeData cubeData;
	//Mesh box(cubeData.vertices, cubeData.indices, cubeData.textures);
	/* ========================================== */

	while (!window.isWindowClosed()) {
		OpenGLWindow::calculateDeltaTime();
		window.processInput();
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		mat4 view = camera.GetViewMatrix();
		mat4 model;

		// =========================绘制其他渲染对象=====================
		shaderGeometryPass.use();
		shaderGeometryPass.setMat4("projection", projection);
		shaderGeometryPass.setMat4("view", view);
		
		gBuffer.switch2Framebuffer();
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (GLuint i = 0; i < objectPositions.size(); i++)
		{
			model = glm::mat4();
			model = glm::translate(model, objectPositions[i]);
			model = glm::scale(model, glm::vec3(0.25f));
			shaderGeometryPass.setMat4("model", model);
			nanosuit.Draw(shaderGeometryPass);
		}
		gBuffer.switch2Defaultbuffer();

		if (selection > 0) {
			showGBufferShader.use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gBuffer.getBuffer(0));
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gBuffer.getBuffer(1));
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, gBuffer.getBuffer(2));
			showGBufferShader.setInt("selection", selection);
			gBuffer.drawFramebuffer2Defaultbuffer(showGBufferShader);
			glActiveTexture(0);
		}

		// =============================================================

		///*Draw skybox*/ {
		//	glDepthFunc(GL_LEQUAL);
		//	skyboxShader.use();
		//	view = mat4(mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		//	skyboxShader.setMat4("view", view);
		//	skyboxShader.setMat4("projection", projection);
		//	box.Draw(skyboxShader);
		//	glDepthFunc(GL_LESS);
		//}
		
		ImGuiHelper::drawImGui(drawGUI);
		window.swapBuffersAndPollEvents();
	}
	return 0;
}


void drawGUI() {
	ImGui::Begin("ToolBox");

	// =======================绘制ImGUI=========================
	static const char* selectionNames[5] = { "lighting","show position","show normal","show albedo","show specular" };
	if (ImGui::TreeNode("Selection")) {
		for (int n = 0; n < _countof(selectionNames); n++) {
			if (ImGui::Selectable(selectionNames[n], selection == n)) {
				selection = n;
			}
		}
		ImGui::TreePop();
	}
	// =========================================================

	/*Draw FPS*/ {
		float frameRate = ImGui::GetIO().Framerate;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / frameRate, frameRate);
	}

	ImGui::End();
}
