//gl相关的头文件有顺序要求
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <LearnOpenGL/shader.h>
#include <LearnOpenGL/camera.h>
#include <LearnOpenGL/opengl_window.h>
#include <LearnOpenGL/assets_directory.h>
#include <LearnOpenGL/texture_loader.h>
#include <LearnOpenGL/model.h>

#include <iostream>
#include <math.h>
#include <string>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

using namespace std;
using namespace glm;

//cube with normal !!true cull
float vertices[] = {
	// positions          // normals           // texture coords
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,


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

	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f
};

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;
Camera myCam(vec3(0, 0, 3));
bool mouse_ctrl;

void processInput(OpenGLWindow &);
void setDirLight(Shader shader);
void setPointLight(Shader shader);
void setSpotLight(Shader shader);
void Drawlamp(Shader lampShader, mat4 model, mat4 projection, mat4 view, unsigned lightVAO);
void DrawGUI();

//DirectionLight
bool isDirLight = true;
vec4 dla(0.05f, 0.05f, 0.05f, 1);
vec4 dld(0.4f, 0.4f, 0.4f, 1);
vec4 dls(0.5f, 0.5f, 0.5f, 1);

//PointLight
bool isPointLight = true;
vec4 pla(0.05f, 0.05f, 0.05f, 1);
vec4 pld(0.8f, 0.8f, 0.8f, 1);
vec4 pls(1.0f, 1.0f, 1.0f, 1);

//SpotLight
bool isSpotLight = true;
vec4 sla(0.0f, 0.0f, 0.0f, 1);
vec4 sld(1.0f, 1.0f, 1.0f, 1);
vec4 sls(1.0f, 1.0f, 1.0f, 1);


glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};


int main() {

	OpenGLWindow window;
	try {
		window.initWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL_Lighting");
	}
	catch (exception e) {
		cout << e.what() << endl;
		return -1;
	}
	window.setCursorEnable();
	window.setCursorPosCallback([](GLFWwindow* window, double xpos, double ypos) {
		if (myCam.firstMouse) {
			lastX = xpos;
			lastY = ypos;
			myCam.firstMouse = false;
		}
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;
		myCam.ProcessMouseMovement(xoffset, yoffset);
	});
	//imgui 1.60的bug，这个函数会失效
	//window.setScrollCallback([](GLFWwindow *window, double xoffset, double yoffset) {
	//	myCam.ProcessMouseScroll(yoffset);
	//});


	Model nanosuit(dir_models + "nanosuit/nanosuit.obj");
	Model basketball(dir_models + "basketball.obj");

	Shader lightingShader(dir_shaders + "StandardPhong.vs", dir_shaders + "StandardPhong.fs");
	Shader lampShader(dir_shaders + "lampShader.vs", dir_shaders + "lampShader.fs");

	unsigned lightVBO, lightVAO;
	glGenBuffers(1, &lightVBO);
	glGenVertexArrays(1, &lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(lightVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window.getWindowPtr(), true);
	ImGui::StyleColorsDark();
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
		mat4 model = mat4(1.0);
		model = translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

		lightingShader.use();
		lightingShader.setVec3("viewPos", myCam.Position);
		//matrix
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("model", model);

		//lightSetting
		setDirLight(lightingShader);
		setSpotLight(lightingShader);
		setPointLight(lightingShader);
		Drawlamp(lampShader, model, projection, view, lightVAO);

		nanosuit.Draw(lightingShader);
		basketball.Draw(lightingShader);

		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		DrawGUI();
		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		window.swapBuffersAndPollEvents();
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &lightVBO);
	return 0;
}

inline void processInput(OpenGLWindow & w)
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
	if(glfwGetMouseButton(pw,GLFW_MOUSE_BUTTON_2)==GLFW_PRESS)
		if (!myCam.mouse_ctrl) {
			w.setCursorDisable();
			myCam.mouse_ctrl = true;
		}
	if (glfwGetMouseButton(pw, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE)
		if (myCam.mouse_ctrl) {
			w.setCursorEnable();
			myCam.mouse_ctrl = false;
		}
}


inline void setDirLight(Shader shader) {
	//directionalLight
	vec3 lightDir(-0.2f, -1.0f, -0.3f);
	shader.setBool("dirLight.isOn", isDirLight);
	shader.setVec3("dirLight.direction", lightDir);
	shader.setVec3("dirLight.ambient", dla.x, dla.y, dla.z);
	shader.setVec3("dirLight.diffuse", dld.x, dld.y, dld.z);
	shader.setVec3("dirLight.specular", dls.x, dls.y, dls.z);
}

inline void setPointLight(Shader shader) {
	for (int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); i++) {
		string lightName("pointLights");
		char c = i + '0';
		lightName = lightName + '[' + c + ']';
		//pointLight
		shader.setBool(lightName + ".isOn", isPointLight);
		shader.setVec3(lightName + ".position", pointLightPositions[i]);
		shader.setVec3(lightName + ".ambient", pla.x, pla.y, pla.z);
		shader.setVec3(lightName + ".diffuse", pld.x, pld.y, pld.z);
		shader.setVec3(lightName + ".specular", pls.x, pls.y, pls.z);
		shader.setFloat(lightName + ".constant", 1.0f);
		shader.setFloat(lightName + ".linear", 0.09f);
		shader.setFloat(lightName + ".quadratic", 0.032f);
	}
}

inline void setSpotLight(Shader shader) {
	//spotLight
	float phi = 12.5f, gamma = 17.5f;
	shader.setBool("spotLight.isOn", isSpotLight);
	shader.setVec3("spotLight.position", myCam.Position);
	shader.setVec3("spotLight.direction", myCam.Front);
	shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(phi)));
	shader.setFloat("spotLight.outterCutOff", glm::cos(glm::radians(gamma)));
	shader.setFloat("spotLight.constant", 1.0f);
	shader.setFloat("spotLight.linear", 0.09f);
	shader.setFloat("spotLight.quadratic", 0.032f);
	shader.setVec3("spotLight.ambient", sla.x, sla.y, sla.z);
	shader.setVec3("spotLight.diffuse", sld.x, sld.y, sld.z);
	shader.setVec3("spotLight.specular", sls.x, sls.y, sls.z);
}

inline void Drawlamp(Shader lampShader,mat4 model,mat4 projection,mat4 view,unsigned lightVAO){
	for (int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]) && isPointLight; i++) {
		model = mat4(1.0f); 
		model = translate(model, pointLightPositions[i]);
		model = scale(model, vec3(0.2f)); // a smaller cube
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		lampShader.setMat4("model", model);
		lampShader.setVec3("lightDiff", pls.x, pls.y, pls.z);
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

inline void DrawGUI(){
	ImGui_ImplGlfwGL3_NewFrame();
	{
		ImGui::Begin("Tools");
		ImGui::Text("Camera Parama");
		ImGui::DragFloat("Yaw", &myCam.Yaw, 0.5f);
		ImGui::DragFloat("Pitch", &myCam.Pitch, 0.5f ,-89.0f, 89.0f);
		ImGui::DragFloat3("Position", value_ptr(myCam.Position), 0.05f);
		ImGui::Text("Because the bug of imgui 1.60,scroll can't change zoom");
		ImGui::SliderFloat("Zoom", &myCam.Zoom, 0.0f, 60.0f);
		if (!myCam.mouse_ctrl) myCam.updateCameraVectors();
		ImGui::Text("Light Selecter");
		ImGui::Checkbox("Open Direction Light", &isDirLight);
		ImGui::Checkbox("Open Point Light", &isPointLight);
		ImGui::Checkbox("Open Spot Light", &isSpotLight);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}