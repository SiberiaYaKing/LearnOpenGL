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

#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace glm;

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

int main()
{
	OpenGLWindow window;{
		try { window.initWindow(SCR_WIDTH, SCR_HEIGHT, "AdvanceOpenGL_CubeMap"); }
		catch (std::exception e) {
			std::cout << e.what() << std::endl;
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
			if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(&window, true);
			if (glfwGetKey(&window, GLFW_KEY_W) == GLFW_PRESS)
				camera.ProcessKeyboard(FORWARD, OpenGLWindow::deltaTime);
			if (glfwGetKey(&window, GLFW_KEY_S) == GLFW_PRESS)
				camera.ProcessKeyboard(BACKWARD, OpenGLWindow::deltaTime);
			if (glfwGetKey(&window, GLFW_KEY_A) == GLFW_PRESS)
				camera.ProcessKeyboard(LEFT, OpenGLWindow::deltaTime);
			if (glfwGetKey(&window, GLFW_KEY_D) == GLFW_PRESS)
				camera.ProcessKeyboard(RIGHT, OpenGLWindow::deltaTime);
			if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
				if (!camera.mouse_ctrl) {
					window.setCursorDisable();
					camera.mouse_ctrl = true;
				}
			if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE)
				if (camera.mouse_ctrl) {
					window.setCursorEnable();
					camera.mouse_ctrl = false;
				}
		});
		window.setScrollCallback([](GLFWwindow* pw, double xoffset, double yoffset) {
			camera.ProcessMouseScroll(yoffset);
		});
	}
	
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
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
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// cube VAO
	unsigned int cubeVAO, cubeVBO; {
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
	}

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO; {
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindVertexArray(0);
	}

	//load skybox image
	//-----------------------------------------------------
	vector<string> face_paths{ "right.jpg","left.jpg","top.jpg","bottom.jpg","front.jpg","back.jpg" };
	for (int i = 0; i < face_paths.size(); i++) 
		face_paths[i] = dir_textures + "skybox/" + face_paths[i];
	TextureLoader skyboxTexture(face_paths);
	TextureLoader cubeTexture(dir_textures + "container.jpg");

	//load cubeShader 
	//---------------
	Shader skyboxShader(dir_shaders + "AdvanceOpenGL/skyboxShader.vs", dir_shaders + "AdvanceOpenGL/skyboxShader.fs");
	Shader cubeShader(dir_shaders + "AdvanceOpenGL/depth_testing.vs", dir_shaders + "AdvanceOpenGL/depth_testing.fs");

	//cubeShader settting
	cubeShader.use();
	cubeShader.setInt("texture1", 0);
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// render loop
	// -----------
	while (!window.isWindowClosed())
	{
		// per-frame time logic
		// --------------------
		OpenGLWindow::deltaTime = OpenGLWindow::calculateDeltaTime();

		// input
		// -----
		window.processInput();

		// render
		// ------
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		TextureLoader::activeTexture(GL_TEXTURE0);
		mat4 model = mat4(1.0f);
		mat4 view = camera.GetViewMatrix();
		mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		/*Draw skybox*/ {
			glDepthMask(GL_FALSE);
			skyboxShader.use();
			view = mat4(mat3(camera.GetViewMatrix()));
			skyboxShader.setMat4("view", view);
			skyboxShader.setMat4("projection", projection);
			glBindVertexArray(skyboxVAO);
			skyboxTexture.bindTexture();
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthMask(GL_TRUE);
		}

		/*Draw cube*/ {
			cubeShader.use();
			view = camera.GetViewMatrix();
			cubeShader.setMat4("model", model);
			cubeShader.setMat4("view", view);
			cubeShader.setMat4("projection", projection);
			glBindVertexArray(cubeVAO);
			cubeTexture.bindTexture();
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		window.swapBuffersAndPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &skyboxVAO);

	return 0;
}




