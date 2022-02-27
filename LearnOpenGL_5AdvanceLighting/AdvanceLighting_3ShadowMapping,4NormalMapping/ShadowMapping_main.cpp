#include "pch.h"

using namespace std;
using namespace glm;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 560;
const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;

Camera camera(glm::vec3(0.0f, 0.1f, 5.0f));

void drawGUI();

const string shaderDir = dir_shaders + "AdvanceLighting/ShadowMapping&NormalMapping/";

struct DirectionLight
{
	DirectionLight() {
		GLfloat near_plane = 1.0f, far_plane = 20.0f;
		glm::vec3 lightPos(4, 4, 4.8);
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		lightSpaceMat = lightProjection * lightView;
	}
	bool isDirLight = true;
	vec3 lightDir = { -0.5f, -0.5f, -0.6f };
	vec3 dla = { 0.4, 0.4, 0.4 };
	vec3 dld = { 0.7, 0.7, 0.7, };
	vec3 dls = { 0.9f, 0.9f, 0.9f,};
	mat4 lightSpaceMat;
};

void RenderScene(vector<glm::vec3>& objectPositions, Model& nanosuit, Mesh& groundMesh, Shader& shader);

bool debugShadowMap = false;
bool useShadowMap = true;
bool useNormalMap = true;
float lightRotateOffsetY = 0;

int main() {
	OpenGLWindow window; {
		try { window.initWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL"); }
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
	//window.enableGamma(true);
	glEnable(GL_CULL_FACE);
	mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	// ========================设置渲染对象=========================

	//perpare model
	Model nanosuit(dir_models + "nanosuit/nanosuit.obj");
	vector<glm::vec3> objectPositions;
	float spacing = 2.0;
	float height = -3.0;
	objectPositions.push_back(glm::vec3(-spacing, height, -spacing));
	objectPositions.push_back(glm::vec3(0.0, height, -spacing));
	objectPositions.push_back(glm::vec3(spacing, height, -spacing));
	objectPositions.push_back(glm::vec3(-spacing, height, 0.0));
	objectPositions.push_back(glm::vec3(0.0, height, 0.0));
	objectPositions.push_back(glm::vec3(spacing, height, 0.0));
	objectPositions.push_back(glm::vec3(-spacing, height, spacing));
	objectPositions.push_back(glm::vec3(0.0, height, spacing));
	objectPositions.push_back(glm::vec3(spacing, height, spacing));

	string groundTexPath = dir_textures + "ground.jpg";
	string specularTexPath = dir_textures + "defaultSpecular.png";
	TextureLoader groundTexLoader(groundTexPath);
	TextureLoader specularTexLoader(specularTexPath);
	QuadData groundQuadData;
	for (Vertex& vertex : groundQuadData.vertices) {
		vertex.TexCoords *= 10;
	}
	groundQuadData.textures.push_back({ groundTexLoader.getTextureID(),TEXTURE_DIFFUSE,groundTexPath });
	groundQuadData.textures.push_back({ specularTexLoader.getTextureID(),TEXTURE_SPECULAR,specularTexPath });
	Mesh groundMesh(groundQuadData.vertices, groundQuadData.indices, groundQuadData.textures);
	
	Shader phongShader(shaderDir + "StandardPhong.vs", shaderDir + "StandardPhong.fs");
	DirectionLight light;
	glm::mat4 originLightSpaceMat = light.lightSpaceMat;
	glm::vec4 originLightDir = vec4(light.lightDir,0);
	// ############# ShadowMap prepare ##########
	ShadowMapFrameBuffer shadowFB(SHADOW_WIDTH, SHADOW_HEIGHT);
	Shader shadowMapShader(shaderDir +"simpleDepthShader.vs", shaderDir +"simpleDepthShader.fs");

	// ============================================================

	/* ==============Set up skybox=============== */ 
	vector<string> face_paths{ "right.jpg","left.jpg","top.jpg","bottom.jpg","front.jpg","back.jpg" };
	for (int i = 0; i < face_paths.size(); i++) {face_paths[i] = dir_textures + "skybox/" + face_paths[i];}
	TextureLoader skyboxTexture(face_paths);
	Shader skyboxShader(dir_shaders + "AdvanceOpenGL/cubemap/skyboxShader.vs", dir_shaders + "AdvanceOpenGL/cubemap/skyboxShader.fs");
	CubeData cubeData;
	Mesh box(cubeData.vertices, cubeData.indices, cubeData.textures);
	/* ========================================== */

	while (!window.isWindowClosed()) {
		OpenGLWindow::calculateDeltaTime();
		window.processInput();
		glClearColor(0, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		

		mat4 view = camera.GetViewMatrix();

		// =========================绘制其他渲染对象=====================
		float lightRotateOffsetYRadians = glm::radians(lightRotateOffsetY);
		light.lightSpaceMat = glm::rotate(originLightSpaceMat, lightRotateOffsetYRadians, glm::vec3(0, 1, 0));
		light.lightDir = glm::rotate(mat4{}, lightRotateOffsetYRadians, glm::vec3(0, 1, 0))*originLightDir;
		shadowMapShader.use();
		shadowMapShader.setMat4("lightSpaceMatrix", light.lightSpaceMat);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		shadowFB.switch2Framebuffer();
		glClear(GL_DEPTH_BUFFER_BIT);
		if (useShadowMap) {
			RenderScene(objectPositions, nanosuit, groundMesh, shadowMapShader);
		} else {
			debugShadowMap = false;
			shadowFB.clearDepth();
		}
		shadowFB.switch2Defaultbuffer();
		

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (debugShadowMap && useShadowMap) {
			glViewport(0, 0, SHADOW_WIDTH / 2, SHADOW_HEIGHT / 2);
			window.resetSize(SHADOW_WIDTH/2, SHADOW_HEIGHT/2);
			shadowFB.drawForShowShadowMap(); //for shadowMap debug
		}
		else if (!debugShadowMap||(debugShadowMap&&!useShadowMap)) {
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			window.resetSize(SCR_WIDTH, SCR_HEIGHT);
			phongShader.use();
			phongShader.setBool("dirLight.isOn", light.isDirLight);
			phongShader.setVec3("dirLight.diffuse", light.dld);
			phongShader.setVec3("dirLight.ambient", light.dla);
			phongShader.setVec3("dirLight.specular", light.dls);
			phongShader.setVec3("dirLight.direction", light.lightDir);
			phongShader.setVec3("viewPos", camera.Position);
			phongShader.setMat4("view", view);
			phongShader.setMat4("projection", projection);
			// setup ShadowMap texture
			phongShader.setMat4("lightSpaceMatrix", light.lightSpaceMat);
			glActiveTexture(GL_TEXTURE0 + groundMesh.textures.size());
			phongShader.setInt("shadowMap", groundMesh.textures.size());
			glBindTexture(GL_TEXTURE_2D, shadowFB.getDepthMapBuffer());

			RenderScene(objectPositions, nanosuit, groundMesh, phongShader);
		}

		// =============================================================

		
		/*Draw skybox*/ {
			glCullFace(GL_FRONT);
			glDepthFunc(GL_LEQUAL);
			skyboxTexture.activeAndBind();
			skyboxShader.use();
			view = mat4(mat3(camera.GetViewMatrix())); // remove translation from the view matrix
			skyboxShader.setMat4("view", view);
			skyboxShader.setMat4("projection", projection);
			box.Draw(skyboxShader);
			skyboxTexture.unactiveAndUnbind();
			glDepthFunc(GL_LESS);
			glCullFace(GL_BACK);
		}
		
		ImGuiHelper::drawImGui(drawGUI);
		window.swapBuffersAndPollEvents();
	}
	return 0;
}

void RenderScene(vector<glm::vec3>& objectPositions, Model& nanosuit, Mesh& groundMesh, Shader& shader) {
	for (int i = 0; i < objectPositions.size(); i++) {
		mat4 modelMat = mat4(1.0f);
		modelMat = glm::translate(modelMat, objectPositions[i]);
		modelMat = glm::scale(modelMat, { 0.2,0.2,0.2 });
		shader.setMat4("model", modelMat);
		nanosuit.Draw(shader);
	}
	//不知道为啥这个平面的法线有点怪...
	glCullFace(GL_FRONT);
	mat4 groundModelMat = mat4(1.0f);
	groundModelMat = glm::translate(groundModelMat, { 0,-3,0 });
	groundModelMat = glm::rotate(groundModelMat, glm::half_pi<float>(), { -1,0,0 });
	groundModelMat = glm::scale(groundModelMat, { 50,50,50 });
	shader.setMat4("model", groundModelMat);
	groundMesh.Draw(shader);
	glCullFace(GL_BACK);
}

void drawGUI() {
	ImGui::Begin("ToolBox");

	// =======================绘制ImGUI=========================
	ImGui::Checkbox("DebugShadowMap", &debugShadowMap);
	ImGui::Checkbox("ShowShadow", &useShadowMap);
	ImGui::SliderFloat("", &lightRotateOffsetY, -180.0f, 180.0f);
	ImGui::Text("LightRotateOffsetY");

	// =========================================================

	/*Draw FPS*/ {
		float frameRate = ImGui::GetIO().Framerate;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / frameRate, frameRate);
	}

	ImGui::End();
}
