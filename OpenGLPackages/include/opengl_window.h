#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <exception>
//Òþ²Ø¿ØÖÆÌ¨´°¿Ú
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

class InitWindowException :public std::exception {
public:
	const char* what()const throw() {
		return "Failed to create GLFW window\n";
	}
};

class InitGLADException :public std::exception {
public:
	const char* what()const throw() {
		return "Failed to initialize GLAD\n";
	}
};

class GLWindow {
private:
	GLFWwindow *window;
public:
	void initWindow(unsigned int width,unsigned int height,const std::string &name);
	void setCursorDisable();
	void setCursorEnable();
	void setCursorPosCallback(GLFWcursorposfun cbfun);
	void setScrollCallback(GLFWscrollfun cbfun);
	void swapBuffersAndPollEvents();
	GLFWwindow* getWindowPtr()const;
	bool isWindowClosed() const;
	~GLWindow();
};

void GLWindow::initWindow(unsigned int width, unsigned int height, const std::string &name) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		throw InitWindowException();
	}

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
	});
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw InitGLADException();
	}
}

GLWindow::~GLWindow() {
	glfwTerminate();
}

void GLWindow::setCursorDisable() {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void GLWindow::setCursorEnable() {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void GLWindow::setCursorPosCallback(GLFWcursorposfun cbfun)  {
	glfwSetCursorPosCallback(window, cbfun);
}

void GLWindow::setScrollCallback(GLFWscrollfun cbfun)  {
	glfwSetScrollCallback(window, cbfun);
}

bool GLWindow::isWindowClosed() const {
	return glfwWindowShouldClose(window);
}

void GLWindow::swapBuffersAndPollEvents()  {
	glfwSwapBuffers(window);
	glfwPollEvents();
}

GLFWwindow* GLWindow::getWindowPtr()const {
	return window;
}


