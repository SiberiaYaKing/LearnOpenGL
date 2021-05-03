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
#include <LearnOpenGL/framebuffer.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#include <iostream>
#include <vector>
#include <string>

void DrawGUI();

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

int screenEffectSelection = 0;

int main()
{
	OpenGLWindow window; {
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

	// build and compile shaders
	// -------------------------
	Shader shader(dir_shaders+"AdvanceOpenGL/depth_testing/depth_testing.vs", dir_shaders+"AdvanceOpenGL/depth_testing/depth_testing.fs");
	Shader default_screenShader(dir_shaders + "AdvanceOpenGL/frame_buffer/frame_buffer.vs", dir_shaders + "AdvanceOpenGL/frame_buffer/frame_buffer.fs");
	Shader inversion_screenShader(dir_shaders + "AdvanceOpenGL/frame_buffer/frame_buffer.vs", dir_shaders + "AdvanceOpenGL/frame_buffer/inversion.fs");
	Shader grayscale_screenShader(dir_shaders + "AdvanceOpenGL/frame_buffer/frame_buffer.vs", dir_shaders + "AdvanceOpenGL/frame_buffer/grayscale.fs");
	Shader kernel_screenShader(dir_shaders + "AdvanceOpenGL/frame_buffer/frame_buffer.vs", dir_shaders + "AdvanceOpenGL/frame_buffer/kernel_processing.fs");

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
	float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
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

	// plane VAO
	unsigned int planeVAO, planeVBO; {
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
	}


	// load textures
	// -------------
	TextureLoader cubeTexture(dir_textures + "container.jpg");
	TextureLoader floorTexture(dir_textures + "metal.png");

	// shader configuration
	// --------------------
	shader.use();
	shader.setInt("texture1", 0);


	Framebuffer framebuffer(SCR_WIDTH, SCR_HEIGHT,0);
	framebuffer.attachBuffer(SCR_WIDTH, SCR_HEIGHT, 0);

	//init GUI
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(&window, true);
	ImGui::StyleColorsDark();

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
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// render
		// ------
		framebuffer.switch2Framebuffer();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*Draw Scene*/{
			shader.use();
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);
			// cubes
			glBindVertexArray(cubeVAO);
			TextureLoader::activeTexture(GL_TEXTURE0);
			cubeTexture.bindTexture();
			model = glm::translate(model, glm::vec3(-1.0f, 0.001f, -1.0f));
			shader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(2.0f, 0.001f, 0.0f));
			shader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// floor
			glBindVertexArray(planeVAO);
			floorTexture.bindTexture();
			shader.setMat4("model", glm::mat4(1.0f));
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}

		framebuffer.switch2Defaultbuffer();
		glClearColor(1.0f, 1.0f, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		/*screen shader setting*/
		switch(screenEffectSelection){
			case 0:
				default_screenShader.use();
				default_screenShader.setInt("screenTexture", 0);
				framebuffer.drawFramebuffer2Defaultbuffer(default_screenShader);
				break;
			case 1:
				inversion_screenShader.use();
				inversion_screenShader.setInt("screenTexture", 0);
				framebuffer.drawFramebuffer2Defaultbuffer(inversion_screenShader);
				break;
			case 2:
				grayscale_screenShader.use();
				grayscale_screenShader.setInt("screenTexture", 0);
				framebuffer.drawFramebuffer2Defaultbuffer(grayscale_screenShader);
				break;
			case 3: case 4: case 5:
				kernel_screenShader.use();
				kernel_screenShader.setInt("screenTexture", 0);
				kernel_screenShader.setInt("selection", screenEffectSelection);
				framebuffer.drawFramebuffer2Defaultbuffer(kernel_screenShader);
				break;
			default: break;
		}
		
		DrawGUI();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		window.swapBuffersAndPollEvents();
	}

	//Close GUI
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	return 0;
}


//一个简单的后处理效果切换
inline void DrawGUI() {
	ImGui_ImplGlfwGL3_NewFrame();
	{
		const char* selectionNames[6] = { "Default","GrayScale","Inversion","Sharpen","Blur","EdgeDetection" };
		ImGui::Begin("Tools");
		if (ImGui::TreeNode("Screen Effect")) {
			for (int n = 0; n < sizeof(selectionNames) / sizeof(const char*); n++) 
				if (ImGui::Selectable(selectionNames[n], screenEffectSelection == n))
					screenEffectSelection = n;
			ImGui::TreePop();
		}
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}


