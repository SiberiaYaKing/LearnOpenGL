#include "pch.h"

using namespace std;
using namespace glm;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 560;

int main() {
	OpenGLWindow window; {
		try { window.initWindow(SCR_WIDTH, SCR_HEIGHT, "PartFor2DRendering_TextRendering"); }
		catch (OpenGLWindowException e) {
			cout << e.what() << endl;
			return -1;
		}
	}

	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	FT_Face face;
	if (FT_New_Face(ft, (dir_fonts+"Arial.ttf").c_str(), 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}

	while (!window.isWindowClosed()) {
		OpenGLWindow::calculateDeltaTime();
		OpenGLWindow::openglClearDefaultFrameBuffer( 0.0, 0.0, 0.5, 1.0);
		window.processInput();


		window.swapBuffersAndPollEvents();
	}
	return 0;
}