#include "pch.h"

using namespace std;
using namespace glm;

const unsigned int SCR_WIDTH = 400;
const unsigned int SCR_HEIGHT = 300;

Camera camera(glm::vec3(0.0f, 0.1f, 3.0f));

void drawGUI();

bool enableMSAA = false;
bool useFrameBuffer = true;
bool enableBlur = true;


struct DirectionLight
{
	bool isDirLight = true;
	vec3 lightDir = { -0.2f, -1.0f, -0.3f };
	vec4 dla = { 0, 1, 0, 1 };
	vec4 dld = { 0, 1, 0, 1 };
	vec4 dls = { 0.9f, 0.9f, 0.9f, 1 };
};

int main() {
	OpenGLWindow window; {
		try { window.initWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL",4); }
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
		window.enableMSAA(enableMSAA);
	}

	mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	// ========================设置渲染对象=========================
	MSAAFrameBuffer framebuffer(SCR_WIDTH, SCR_HEIGHT, 1,4);
	Framebuffer intermediateFB(SCR_WIDTH, SCR_HEIGHT,1);

	CubeData boxData;
	Mesh box(boxData.vertices, boxData.indices, boxData.textures);
	Shader boxShader(dir_shaders + "StandardPhong.vs", dir_shaders + "StandardPhong.fs");
	Shader default_screenShader(dir_shaders + "AdvanceOpenGL/frame_buffer/frame_buffer.vs", dir_shaders + "AdvanceOpenGL/frame_buffer/frame_buffer.fs");
	Shader kernel_screenShader(dir_shaders + "AdvanceOpenGL/frame_buffer/frame_buffer.vs", dir_shaders + "AdvanceOpenGL/frame_buffer/kernel_processing.fs");
	glEnable(GL_DEPTH_TEST);
	// ============================================================

	/* ==============Set up skybox=============== */ 
	vector<string> face_paths{ "right.jpg","left.jpg","top.jpg","bottom.jpg","front.jpg","back.jpg" };
	for (int i = 0; i < face_paths.size(); i++) {face_paths[i] = dir_textures + "skybox/" + face_paths[i];}
	TextureLoader skyboxTexture(face_paths);
	Shader skyboxShader(dir_shaders + "AdvanceOpenGL/cubemap/skyboxShader.vs", dir_shaders + "AdvanceOpenGL/cubemap/skyboxShader.fs");
	CubeData cubeData;
	Mesh skyBox(cubeData.vertices, cubeData.indices, cubeData.textures);
	/* ========================================== */

	while (!window.isWindowClosed()) {
		OpenGLWindow::calculateDeltaTime();
		window.processInput();

		if (useFrameBuffer) {
			framebuffer.switch2Framebuffer();
		}

		glClearColor(1, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		mat4 view = camera.GetViewMatrix();

		// =========================绘制其他渲染对象=====================
		mat4 model = mat4(1.0f);
		DirectionLight light;
		boxShader.use();
		boxShader.setMat4("model", model);
		boxShader.setMat4("view", view);
		boxShader.setMat4("projection", projection);
		boxShader.setVec3("viewPos", camera.Position);
		boxShader.setBool("dirLight.isOn", light.isDirLight);
		boxShader.setVec3("dirLight.diffuse", light.dld[0],light.dld[1],light.dld[2]);
		boxShader.setVec3("dirLight.ambient", light.dla[0],light.dla[1],light.dla[2]);
		boxShader.setVec3("dirLight.specular",light.dls[0],light.dls[1],light.dls[2]);
		boxShader.setVec3("dirLight.direction", light.lightDir);
		box.Draw(boxShader);

		// =============================================================


		/*Draw skybox*/ {
			glDepthFunc(GL_LEQUAL);
			skyboxTexture.activeAndBind();
			skyboxShader.use();
			view = mat4(mat3(camera.GetViewMatrix())); // remove translation from the view matrix
			skyboxShader.setMat4("view", view);
			skyboxShader.setMat4("projection", projection);
			skyBox.Draw(skyboxShader);
			skyboxTexture.unactiveAndUnbind();
			glDepthFunc(GL_LESS);
		}
		
		if (useFrameBuffer) {
			//now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO.Image is stored in screenTexture
			glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.getFrameBuffer());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFB.getFrameBuffer());
			glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			intermediateFB.switch2Defaultbuffer();
			glClearColor(0.0f, 0.0f, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
			intermediateFB.bindColorBuffer();
			if (enableBlur) {
				kernel_screenShader.use();
				kernel_screenShader.setInt("screenTexture", 0);
				kernel_screenShader.setInt("selection", 4);
				intermediateFB.drawFramebuffer2Defaultbuffer(kernel_screenShader);
			}
			else {
				default_screenShader.use();
				default_screenShader.setInt("screenTexture", 0);
				intermediateFB.drawFramebuffer2Defaultbuffer(default_screenShader);
			}
			glActiveTexture(0);
		}
		window.enableMSAA(enableMSAA);


		ImGuiHelper::drawImGui(drawGUI);
		window.swapBuffersAndPollEvents();
	}
	return 0;
}

void drawGUI() {
	ImGui::Begin("ToolBox");

	// =======================绘制ImGUI=========================
	ImGui::Checkbox("useFrameBuffer", &useFrameBuffer);
	if (useFrameBuffer) {
		ImGui::Checkbox("enableBlur", &enableBlur);
	}
	ImGui::Checkbox("enableMSAA", &enableMSAA);
	
	// =========================================================

	/*Draw FPS*/ {
		float frameRate = ImGui::GetIO().Framerate;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / frameRate, frameRate);
	}

	ImGui::End();
}
