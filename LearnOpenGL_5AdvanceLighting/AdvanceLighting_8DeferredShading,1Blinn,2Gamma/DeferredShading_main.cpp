#include <pch.h>

using namespace std;
using namespace glm;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 560;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
const string localShaderDir = dir_shaders + ("AdvanceLighting/DeferredShading/");

int selection = 0;
bool gammaCorrect = true;
int lightingMode = 0;

void drawGUI();

int main() {
	OpenGLWindow window; {
		try { window.initWindow(SCR_WIDTH, SCR_HEIGHT, "AdvanceLighting_DeferredShading&Blinn&Gamma",4); }
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
	// prepare Gbuffer
	Framebuffer gBuffer(SCR_WIDTH, SCR_HEIGHT,0);
	// position buffer
	gBuffer.attachBuffer(SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB16F, GL_RGB, GL_FLOAT);
	// normal buffer
	gBuffer.attachBuffer(SCR_WIDTH, SCR_HEIGHT, 1, GL_RGB16F, GL_RGB, GL_FLOAT);
	// albedo + specular buffer
	gBuffer.attachBuffer(SCR_WIDTH, SCR_HEIGHT, 2, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	Shader shaderGeometryPass(localShaderDir + "gBufferShader.vs", localShaderDir + "gBufferShader.fs");

	//perpare model
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

	Shader lampShader(dir_shaders + "lampShader.vs", dir_shaders + "lampShader.fs");
	CubeData cubedata;
	Mesh lampCube(cubedata.vertices, cubedata.indices, cubedata.textures);

	// perpare Lights
	const GLuint NR_LIGHTS = 32;
	vector<vec3> lightPositions;
	vector<vec3> lightColors;
	srand(13);
	for (GLuint i = 0; i < NR_LIGHTS; i++)
	{
		// Calculate slightly random offsets
		GLfloat xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		GLfloat yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		GLfloat zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// Also calculate random color
		GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	// prepare Shader
	Shader showGBufferShader(dir_shaders + "AdvanceOpenGL/frame_buffer/frame_buffer.vs", localShaderDir+"showGBuffer.fs");
	showGBufferShader.use();
	showGBufferShader.setInt("positionTex", 0);
	showGBufferShader.setInt("normalTex", 1);
	showGBufferShader.setInt("albedoSpecTex", 2);
	// ============================================================

	/* ==============Set up skybox=============== */ 
	vector<string> face_paths{ "right.jpg","left.jpg","top.jpg","bottom.jpg","front.jpg","back.jpg" };
	for (int i = 0; i < face_paths.size(); i++) {face_paths[i] = dir_textures + "skybox/" + face_paths[i];}
	TextureLoader skyboxTexture(face_paths);
	Shader skyboxShader(dir_shaders + "AdvanceOpenGL/cubemap/skyboxShader.vs", dir_shaders + "AdvanceOpenGL/cubemap/skyboxShader.fs");
	CubeData cubeData;
	Mesh box(cubeData.vertices, cubeData.indices, cubeData.textures);
	/* ========================================== */

	const float constant = 1.0;
	const float linear = 0.7;
	const float quadratic = 1.8;

	while (!window.isWindowClosed()) {
		OpenGLWindow::calculateDeltaTime();
		window.processInput();
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		mat4 view = camera.GetViewMatrix();
		mat4 model;

		// =========================绘制其他渲染对象=====================
		
		/*Gbuffer pass*/
		gBuffer.switch2Framebuffer();
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderGeometryPass.use();
		shaderGeometryPass.setMat4("projection", projection);
		shaderGeometryPass.setMat4("view", view);
		for (GLuint i = 0; i < objectPositions.size(); i++)
		{
			model = glm::mat4();
			model = glm::translate(model, objectPositions[i]);
			model = glm::scale(model, glm::vec3(0.25f));
			shaderGeometryPass.setMat4("model", model);
			nanosuit.Draw(shaderGeometryPass);
		}
		gBuffer.switch2Defaultbuffer();
		glClearColor(0.0f, 0.0f, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/*draw framebuffer*/
		gBuffer.bindColorBuffer();
		showGBufferShader.use();
		showGBufferShader.setInt("selection", selection);
		showGBufferShader.setBool("gammaCorrect", gammaCorrect);
		showGBufferShader.setInt("lightingMode", lightingMode);
		if(selection==0) {
			for (GLuint i = 0; i < NR_LIGHTS; i++) {
				showGBufferShader.setVec3("lights[" + to_string(i) + "].Position", lightPositions[i]);
				showGBufferShader.setVec3("lights[" + to_string(i) + "].Color", lightColors[i]);
				
				showGBufferShader.setFloat("lights[" + to_string(i) + "].Linear", linear);
				showGBufferShader.setFloat("lights[" + to_string(i) + "].Quadratic", quadratic);

				
				GLfloat lightMax = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
				GLfloat radius =(-linear + std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * lightMax))) / (2 * quadratic);
				showGBufferShader.setFloat("lights[" + to_string(i) + "].Radius", radius);
			}
			showGBufferShader.setVec3("viewPos", camera.Position);
		}
		gBuffer.drawFramebuffer2Defaultbuffer(showGBufferShader);
		glActiveTexture(0);

		/*copy gbuffer's depth to default framebuffer's depth*/
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.getFrameBuffer());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/*Draw lamp object*/
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		for (GLuint i = 0; i < NR_LIGHTS; i++)
		{
			model = translate(mat4(), lightPositions[i]);
			model = scale(model, glm::vec3(0.25f));
			lampShader.setMat4("model", model);
			lampShader.setVec3("lightDiff", lightColors[i]);
			lampCube.Draw(lampShader);
		}

		// =============================================================

		/*Draw skybox*/ {
			glDepthFunc(GL_LEQUAL);
			skyboxTexture.activeAndBind();
			skyboxShader.use();
			view = mat4(mat3(camera.GetViewMatrix())); // remove translation from the view matrix
			skyboxShader.setMat4("view", view);
			skyboxShader.setMat4("projection", projection);
			box.Draw(skyboxShader);
			skyboxTexture.unactiveAndUnbind();
			glDepthFunc(GL_LESS);
		}
		
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
	static const char* lightingModeNames[2] = { "Phong","Blinn" };
	if (ImGui::TreeNode("LightingMode")) {
		for (int n = 0; n < _countof(lightingModeNames); n++) {
			if (ImGui::Selectable(lightingModeNames[n], lightingMode == n)) {
				lightingMode = n;
			}
		}
		ImGui::TreePop();
	}
	ImGui::Checkbox("GammaCorrect", &gammaCorrect);
	// =========================================================

	/*Draw FPS*/ {
		float frameRate = ImGui::GetIO().Framerate;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / frameRate, frameRate);
	}

	ImGui::End();
}
