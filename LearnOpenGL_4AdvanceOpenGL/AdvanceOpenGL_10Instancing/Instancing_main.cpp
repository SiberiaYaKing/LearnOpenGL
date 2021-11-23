#include <pch.h>

using namespace std;
using namespace glm;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 560;

Camera camera(glm::vec3(0.0f, 2.0f, 5.0f), {0,1,0}, YAW, -10.0);
const string deferredShaderDir = dir_shaders + ("AdvanceLighting/DeferredShading/");
const string localShaderDir = dir_shaders + ("AdvanceOpenGL/Instancing/");

int selection = 0;
int level = 8;
int oldLevel = 0;
bool useInstancing = true;
bool gammaCorrect = true;

void drawGUI();
void updateLevel(vector<mat4>& modelMatrices,const Model& nanosuit);

struct DirectionLight
{
	vec3 lightDir = { -0.2f, -1.0f, -0.3f };
	vec3 dla = { 0, 0, 0 };
	vec3 dld = { 0.3, 0.3, 0.3 };
	vec3 dls = { 0.5f, 0.5f, 0.5f};
};

int main() {
	OpenGLWindow window; {
		try { window.initWindow(SCR_WIDTH, SCR_HEIGHT, "AdvanceOpenGL_Instancing", 4); }
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
	Framebuffer gBuffer(SCR_WIDTH, SCR_HEIGHT, 0);
	// position buffer
	gBuffer.attachBuffer(SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB16F, GL_RGB, GL_FLOAT);
	// normal buffer
	gBuffer.attachBuffer(SCR_WIDTH, SCR_HEIGHT, 1, GL_RGB16F, GL_RGB, GL_FLOAT);
	// albedo + specular buffer
	gBuffer.attachBuffer(SCR_WIDTH, SCR_HEIGHT, 2, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	Shader shaderGeometryPass(deferredShaderDir + "gBufferShader.vs", deferredShaderDir + "gBufferShader.fs");
	Shader shaderGeometryPassInstencing(localShaderDir+"gBufferShader_Instancing.vs", deferredShaderDir + "gBufferShader.fs");

	//perpare model
	Model nanosuit(dir_models + "nanosuit/nanosuit.obj");

	vector<mat4> modelMatrices;

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
		GLfloat yPos = ((rand() % 100) / 100.0) * 5.0 - 4.0;
		GLfloat zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// Also calculate random color
		GLfloat rColor = clamp(((rand() % 100) / 200.0f) + 0.8, 0.8, 1.0); // Between 0.8 and 1.0
		GLfloat gColor = clamp(((rand() % 100) / 200.0f) + 0.8, 0.8, 1.0); // Between 0.8 and 1.0
		GLfloat bColor = clamp(((rand() % 100) / 200.0f) + 0.8, 0.8, 1.0); // Between 0.8 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	// prepare Shader
	Shader showGBufferShader(dir_shaders + "AdvanceOpenGL/frame_buffer/frame_buffer.vs", deferredShaderDir + "showGBuffer.fs");
	showGBufferShader.use();
	showGBufferShader.setInt("positionTex", 0);
	showGBufferShader.setInt("normalTex", 1);
	showGBufferShader.setInt("albedoSpecTex", 2);

	DirectionLight dirLight;
	// ============================================================

	/* ==============Set up skybox=============== */
	vector<string> face_paths{ "right.jpg","left.jpg","top.jpg","bottom.jpg","front.jpg","back.jpg" };
	for (int i = 0; i < face_paths.size(); i++) { face_paths[i] = dir_textures + "skybox/" + face_paths[i]; }
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

		// =========================绘制其他渲染对象=====================
		updateLevel(modelMatrices, nanosuit);
		/*Gbuffer pass*/
		gBuffer.switch2Framebuffer();
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (!useInstancing) {
			shaderGeometryPass.use();
			shaderGeometryPass.setMat4("projection", projection);
			shaderGeometryPass.setMat4("view", view);
			for (GLuint i = 0; i < modelMatrices.size(); i++)
			{
				mat4 model = modelMatrices[i];
				shaderGeometryPass.setMat4("model", model);
				nanosuit.Draw(shaderGeometryPass);
			}
		}
		else if(useInstancing) {
			shaderGeometryPassInstencing.use();
			shaderGeometryPassInstencing.setMat4("projection", projection);
			shaderGeometryPassInstencing.setMat4("view", view);
			nanosuit.drawInstance(shaderGeometryPassInstencing, modelMatrices.size());
		}
		gBuffer.switch2Defaultbuffer();
		glClearColor(0.0f, 0.0f, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/*draw framebuffer*/
		gBuffer.bindColorBuffer();
		showGBufferShader.use();
		showGBufferShader.setInt("selection", selection);
		showGBufferShader.setVec3("dirLight.direction", dirLight.lightDir);
		showGBufferShader.setVec3("dirLight.diffuse", dirLight.dld);
		showGBufferShader.setVec3("dirLight.ambient", dirLight.dla);
		showGBufferShader.setVec3("dirLight.specular", dirLight.dls);
		showGBufferShader.setBool("gammaCorrect", gammaCorrect);
		if (selection == 0) {
			for (GLuint i = 0; i < NR_LIGHTS; i++) {
				showGBufferShader.setVec3("lights[" + to_string(i) + "].Position", lightPositions[i]);
				showGBufferShader.setVec3("lights[" + to_string(i) + "].Color", lightColors[i]);

				showGBufferShader.setFloat("lights[" + to_string(i) + "].Linear", linear);
				showGBufferShader.setFloat("lights[" + to_string(i) + "].Quadratic", quadratic);


				GLfloat lightMax = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
				GLfloat radius = (-linear + std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * lightMax))) / (2 * quadratic);
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
			mat4 model = translate(mat4(), lightPositions[i]);
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

void updateLevel(vector<mat4>& modelMatrices,const Model& nanosuit) {
	if (oldLevel != level) {
		oldLevel = level;
		//postionOffset
		int step = 3;
		int size = level * 2 - 1;
		vec3 originPos = vec3(-3.0*(size/2), -3.0, -3.0 *(size/2));
		vector<glm::vec3> objectPositions;
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				objectPositions.push_back(originPos + vec3(step*i, 0, step*j));
			}
		}
		modelMatrices.clear();
		//generate list of modelMatrices
		for (GLuint i = 0; i < objectPositions.size(); i++)
		{
			glm::mat4 model = glm::mat4();
			model = glm::translate(model, objectPositions[i]);
			model = glm::scale(model, glm::vec3(0.25f));
			modelMatrices.push_back(model);
		}
		// setup modelMatrices
		unsigned int modelMatricesBuffer;
		glGenBuffers(1, &modelMatricesBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, modelMatricesBuffer);
		glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(mat4), modelMatrices.data(), GL_STATIC_DRAW);
		// setup VAO , add modelmatrix pointer
		vector<Mesh> nanosuitMeshes;
		nanosuit.getMeshes(nanosuitMeshes);
		for (int i = 0; i < nanosuitMeshes.size(); i++) {
			unsigned int tempVAO = nanosuitMeshes[i].getVAO();
			glBindVertexArray(tempVAO);
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}
	}
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
	ImGui::Checkbox("UseInstancing", &useInstancing);
	ImGui::Checkbox("GammaCorrect", &gammaCorrect);
	ImGui::InputInt("Level", &level);
	if (level > 15) { level = 15; }
	// =========================================================

	/*Draw FPS*/ {
		float frameRate = ImGui::GetIO().Framerate;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / frameRate, frameRate);
	}

	ImGui::End();
}
