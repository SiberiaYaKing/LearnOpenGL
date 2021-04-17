
#include "pch.h"
using namespace std;
using namespace glm;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 560;
const string shaderDir = dir_shaders + ("PartFor2DRendering/TextRendering/");

int main() {
	OpenGLWindow window; {
		try { window.initWindow(SCR_WIDTH, SCR_HEIGHT, "PartFor2DRendering_TextRendering"); }
		catch (OpenGLWindowException e) {
			cout << e.what() << endl;
			return -1;
		}
		window.setInputProcessor([](const OpenGLWindow& window) {
			if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
				{window.closeWindow();}
		});
	}

	Shader textShader(shaderDir + "textShader.vs", shaderDir+ "textShader.fs");
	mat4 projection = ortho(0.0, (double)SCR_WIDTH, 0.0, (double)SCR_HEIGHT);
	textShader.use();
	textShader.setMat4("projection", projection);
	Text text(dir_fonts + "msyh.ttc");

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!window.isWindowClosed()) {
		OpenGLWindow::calculateDeltaTime();
		glClearColor(0.0, 0.0, 0.5, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		window.processInput();

		text.drawText(textShader, L"Bad apple!!!", glm::vec4(0.5, 0.8f, 0.2f, 1.0), { 25.0f, 25.0f }, 1.0f);
		text.drawText(textShader, L"此生无悔入东方", glm::vec4(0.3, 0.7f, 0.9f,1.0f), { 500.0f, 530.0f }, 0.5f);
		

		window.swapBuffersAndPollEvents();
	}
	return 0;
}